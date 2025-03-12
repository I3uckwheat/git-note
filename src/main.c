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
    .description = "edit a note 'git-note -e [branch] [note id]"},
};

int main(int argc, char *argv[]) {
    char repo_name[256];
    GitHelpers__get_dir_name(repo_name, sizeof(repo_name));
    char* retrieved = Storage__retrieve_serialized_table(repo_name);
    HashNote_Table* table = HashNote__deserialize(retrieved);
    free(retrieved);

    char branch_name[256];
    GitHelpers__get_branch_name(branch_name, sizeof(branch_name));


    cag_option_context context;
    cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

    // NOTE: Using goto to break out of the loop and clean up on certain scenarios
    while(cag_option_fetch(&context)) {
        switch(cag_option_get_identifier(&context)) {
            case 'h': {
                printf("Usage: git-note [OPTION]...\n");
                cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
                break;
            }

            // FIXME: When note file is empty or missing, we get a segfault
            case 'l': {
                const char* branch_name = cag_option_get_value(&context);
                if(branch_name) {
                    Display__list_notes(table, branch_name);
                } else {
                    Display__list_branches(table);
                }
                goto end;
            }

            // TODO: if no note, open editor
            case 'a': {
                const char* added_note = cag_option_get_value(&context);
                HashNote__create_new_note(table, branch_name, added_note);
                char* serialized_table = HashNote__serialize_table(table);
                Storage__store_serialized_table(serialized_table, repo_name);
                free(serialized_table);
                goto end;
            }

            case 'e': {
                const char* branch_name = cag_option_get_value(&context);
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

                HashNote_Note* note = HashNote__get_note(table, branch_name, note_id);
                char* edited_note_text = edit_with_git_editor(note->text);
                size_t note_len = strlen(edited_note_text);

                free(note->text);
                note->text = edited_note_text; 
                note->modified_at = time(NULL);

                char* serialized_table = HashNote__serialize_table(table);
                Storage__store_serialized_table(serialized_table, repo_name);
                free(serialized_table);

                goto end;
            }

            case '?': {
                cag_option_print_error(&context, stdout);
                return 1;
            }
        }
    }

    end:
    free(table);
}
