#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include <cargs.h>

#include "hashnote_table.h"
#include "storage.h"
#include "git_helpers.h"
#include "display_notes.h"

#include "file_helpers.h"
#include "string_helpers.h"

static struct cag_option options[] = {
    {.identifier = 'l',
    .access_letters = "l",
    .access_name = NULL,
    .value_name = "branch",
    .description = "list notes"},

    {.identifier = 'h',
    .access_letters = "h",
    .access_name = "help",
    .description = "shows the command help"},

    {.identifier = 'a',
    .access_letters = "a",
    .access_name = "add",
    .value_name = "note",
    .description = "add a note 'git-note -a \"hello world!\"'"},

    {.identifier = 'e',
    .access_letters = "e",
    .access_name = "edit",
    .value_name = "note",
    .description = "edit a note 'git-note -e [branch] [note id]`"},

    {.identifier = 'd',
    .access_letters = "d",
    .access_name = "delete",
    .value_name = "note",
    .description = "delete a note 'git-note -d [branch] [note id]` or omit note id for branch deletion (need to pass --confirm for confirmation)"},

    {.identifier = 'c',
    .access_letters = NULL,
    .access_name = "confirm",
    .value_name = NULL,
    .description = "confirmation flag, used for deletions, see -d"},

    {.identifier = 'b',
    .access_letters = "b",
    .access_name = "branch",
    .value_name = "branch_name",
    .description = "set the branch name for any operation, can be used to add notes to other branches"},
};

enum Operation {
    None,
    Help,
    Add_Note,
    Edit_Note,
    Delete_Note,
    Delete_Branch,
    List_Branch,
    List_All,
};

typedef struct Config {
    enum Operation operation;
    char repo_name[256];
    char branch_name[256];
    const char* added_note;
    size_t note_id;
    bool confirmed;
    HashNote_Table* table;
    bool dirty;
} Config;

static Config config;

// Setup defaults
void setup_config() {
    config.confirmed = false;
    config.operation = None;
    config.dirty = false;
    GitHelpers__get_dir_name(config.repo_name, sizeof(config.repo_name));
    GitHelpers__get_branch_name(config.branch_name, sizeof(config.branch_name));

    char* retrieved = Storage__retrieve_serialized_table(config.repo_name);
    config.table = HashNote__deserialize(retrieved);
    free(retrieved);
}

// TODO: if no note, open editor
void add_note() {
    HashNote__create_new_note(config.table, config.branch_name, config.added_note); 
    config.added_note = NULL;
    config.dirty = true;
}

void edit_note() {
    HashNote_Note* note = HashNote__get_note(config.table, config.branch_name, config.note_id);
    char* edited_note_text = edit_with_git_editor(note->text);

    free(note->text);
    note->text = edited_note_text; 
    note->modified_at = time(NULL);
    config.dirty = true;
}

void delete_note() {
    HashNote_Table__delete_note(config.table, config.branch_name, config.note_id);
    config.dirty = true;
}

void execute() {
    switch(config.operation) {
        case List_All:
            Display__list_branches(config.table);
            break;
        case List_Branch:
            Display__list_notes(config.table, config.branch_name);
            break;
        case Add_Note:
            add_note();
            break;
        case Edit_Note:
            edit_note();
            break;
        case Delete_Note:
            delete_note();
            break;
        case Delete_Branch:
            printf("Not implemented\n");
        default:
            break;
    }

    if(config.dirty) {
        char* serialized_table = HashNote__serialize_table(config.table);
        Storage__store_serialized_table(serialized_table, config.repo_name);
        free(serialized_table);
    }
}

void parse_flags(int argc, char *argv[]) {
    cag_option_context context;
    cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
    while(cag_option_fetch(&context)) { 
        char option_identifier = cag_option_get_identifier(&context);

        // Only set these if we haven't set an operation yet
        if(config.operation == None) {
            switch (option_identifier) {
            case 'h': {
                config.operation = Help;
                printf("Usage: git-note [OPTION]...\n");
                cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
                break;
            }

            case 'l': {
                const char* branch_name = cag_option_get_value(&context);
                if(!branch_name) {
                    config.operation = List_All;
                } else {
                    config.operation = List_Branch;
                    strncpy(config.branch_name, branch_name, sizeof(config.branch_name));
                }
                break;
            }

            case 'e': {
                config.operation = Edit_Note;
                const char* branch_name = cag_option_get_value(&context);
                strncpy(config.branch_name, branch_name, sizeof(config.branch_name));

                char* note_id_string = argv[cag_option_get_index(&context)];
                config.note_id = String__parse_note_id_string(note_id_string);
                break;

            }

            case 'd': {
                config.operation = Delete_Note;
                const char* branch_name = cag_option_get_value(&context);
                strncpy(config.branch_name, branch_name, sizeof(config.branch_name));

                char* note_id_string = argv[cag_option_get_index(&context)];
                config.note_id = String__parse_note_id_string(note_id_string);
                break;
            }

            case 'a': {
                config.operation = Add_Note;
                const char* added_note = cag_option_get_value(&context);
                config.added_note = added_note;
                break;
            }
            
            default:
                break;
            }
        }

        // These can be set anytime
        switch(option_identifier) {
            case 'b': {
                const char* branch_name = cag_option_get_value(&context);
                strncpy(config.branch_name, branch_name, sizeof(config.branch_name));
                break;
            }

            case 'c': {
                config.confirmed = true;
                break;
            }

            case '?': {
                cag_option_print_error(&context, stdout);
                exit(1);
            }
        }
    }

    // TODO: second parameter displays note details
    if(config.operation == None) {
        int param_index = 0;
        param_index = cag_option_get_index(&context);

        // first param is branch
        char* branch_name = argv[param_index];
        if(branch_name) {
            strncpy(config.branch_name, branch_name, sizeof(config.branch_name));
            config.operation = List_Branch;
        } else {
            config.operation = List_All;
        }
    }
}

void cleanup() {
    free(config.table);
}

int main(int argc, char *argv[]) {
    setup_config();
    parse_flags(argc, argv);
    execute();
    cleanup();
}
