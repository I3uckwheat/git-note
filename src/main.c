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

    // TODO: Set a -b for adding notes to other branches
    // {.identifier = 'c',
    // .access_letters = NULL,
    // .access_name = "confirm",
    // .value_name = NULL,
    // .description = "confirmation flag, used for deletions, see -d"},
};

enum Operation {
    Add_Note,
    Edit_Note,
    Delete_Note,
    Delete_Branch,
    List_Branch,
    List_All,
};

// TODO: Add a "needs_save" bool
typedef struct Config {
    enum Operation operation;
    char repo_name[256];
    char branch_name[256];
    // char* added_note;
    size_t note_id;
    bool confirmed;
    HashNote_Table* table;
} Config;

static Config config;

// Setup defaults
void setup_config() {
    config.confirmed = false;
    config.operation = List_All;
    GitHelpers__get_dir_name(config.repo_name, sizeof(config.repo_name));
    GitHelpers__get_branch_name(config.branch_name, sizeof(config.branch_name));

    char* retrieved = Storage__retrieve_serialized_table(config.repo_name);
    config.table = HashNote__deserialize(retrieved);
    free(retrieved);
}

void list_notes() {
    if(config.operation == List_Branch) {
        Display__list_notes(config.table, config.branch_name);
    } else {
        Display__list_branches(config.table);
    }
}

// TODO: if no note, open editor
void add_note(const char* added_note) {
    HashNote__create_new_note(config.table, config.branch_name, added_note);
    char* serialized_table = HashNote__serialize_table(config.table);
    Storage__store_serialized_table(serialized_table, config.repo_name);
    free(serialized_table);
}

void edit_note() {
    HashNote_Note* note = HashNote__get_note(config.table, config.branch_name, config.note_id);
    char* edited_note_text = edit_with_git_editor(note->text);

    free(note->text);
    note->text = edited_note_text; 
    note->modified_at = time(NULL);

    char* serialized_table = HashNote__serialize_table(config.table);
    Storage__store_serialized_table(serialized_table, config.repo_name);
    free(serialized_table);
}

void delete_note() {
    HashNote_Table__delete_note(config.table, config.branch_name, config.note_id);
    char* serialized_table = HashNote__serialize_table(config.table);
    Storage__store_serialized_table(serialized_table, config.repo_name);
    free(serialized_table);
}

int main(int argc, char *argv[]) {
    setup_config();

    cag_option_context context;
    cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
    while(cag_option_fetch(&context)) { // NOTE: Using goto to break out of the loop and clean up on certain scenarios
        switch(cag_option_get_identifier(&context)) {
            case 'h': {
                printf("Usage: git-note [OPTION]...\n");
                cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
                break;
            }

            // FIXME: When note file is empty or missing, we get a segfault
            case 'l': {
                const char* branch_name = cag_option_get_value(&context);
                if(!branch_name) {
                    config.operation = List_All;
                } else {
                    config.operation = List_Branch;
                    strncpy(config.branch_name, branch_name, sizeof(config.branch_name));
                }

                list_notes();
                goto end;
            }

            case 'a': {
                config.operation = Add_Note;
                const char* added_note = cag_option_get_value(&context);
                add_note(added_note);
                goto end;
            }

            case 'e': {
                config.operation = Edit_Note;
                const char* branch_name = cag_option_get_value(&context);
                strncpy(config.branch_name, branch_name, sizeof(config.branch_name));

                char* note_id_string = argv[cag_option_get_index(&context)];
                if(note_id_string == NULL) {
                    printf("Must enter a note_id: git-note [branch] [note id]\n");
                    goto end;
                }

                const size_t note_id = (int)strtol(note_id_string, NULL, 10);

                if(note_id == 0) {
                    printf("Invalid note id: %s\n", note_id_string);
                    goto end;
                }

                config.note_id = note_id;

                edit_note();

                goto end;
            }

            case 'd': {
                const char* branch_name = cag_option_get_value(&context);
                strncpy(config.branch_name, branch_name, sizeof(config.branch_name));

                char* note_id_string = argv[cag_option_get_index(&context)];

                if(note_id_string == NULL) {
                    printf("Must enter a note_id: git-note [branch] [note id]\n");
                    goto end;
                }

                const size_t note_id = (int)strtol(note_id_string, NULL, 10);

                if(note_id == 0) {
                    printf("Invalid note id: %s\n", note_id_string);
                    goto end;
                }

                config.note_id = note_id;

                delete_note();

                goto end;
            }

            case '?': {
                cag_option_print_error(&context, stdout);
                return 1;
            }
        }
    }

    end:
    free(config.table);
}
