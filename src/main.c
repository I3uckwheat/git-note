#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include <cargs.h>

#include "add.h"
#include "hashnote_table.h"
#include "storage.h"
#include "git_helpers.h"
#include "display_notes.h"

static struct cag_option options[] = {
    {.identifier = 'l',
    .access_letters = "l",
    .access_name = NULL,
    .value_name = "branch",
    .description = "list notes"},

    {.identifier = 'h',
    .access_letters = "h",
    .access_name = "help",
    .description = "shows the command help"}
};

int main(int argc, char *argv[]) {
    char repo_name[256];
    GitHelpers__get_dir_name(repo_name, sizeof(repo_name));
    char* retrieved = Storage__retrieve_serialized_table(repo_name);
    HashNote_Table* table = HashNote__deserialize(retrieved);


    cag_option_context context;
    cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
    while(cag_option_fetch(&context)) {
        switch(cag_option_get_identifier(&context)) {
            case 'h':
                printf("Usage: git-note [OPTION]...\n");
                cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
                break;
            case 'l': {
                const char* branch_name = cag_option_get_value(&context);
                if(branch_name) {
                    Display__list_notes(table, branch_name);
                } else {
                    Display__list_branches(table);
                }
                break;
            }
            case '?':
                cag_option_print_error(&context, stdout);
                return 1;
        }
    }

    // // HashNote_Table* table = HashNote__create_table();

    // // HashNote__create_new_note(table, "hello", "one");
    // // HashNote__create_new_note(table, "hello", "two");
    // // HashNote__create_new_note(table, "hello", "three");
    // // HashNote__create_new_note(table, "hello", "four");

    // // HashNote__create_new_note(table, "foo", "bar");
    // // HashNote__create_new_note(table, "foo", "baz");
    // // HashNote__create_new_note(table, "foo", "not a");
    // // HashNote__create_new_note(table, "foo", "weenie");

    // char repo_name[256];
    // GitHelpers__get_dir_name(repo_name, sizeof(repo_name));
    // // char* serialized_table = HashNote__serialize_table(table);
    // // Storage__store_serialized_table(serialized_table, repo_name);

    // // char repo_name[256];
    // // GitHelpers__get_dir_name(repo_name, sizeof(repo_name));
    // char* retrieved = Storage__retrieve_serialized_table(repo_name);
    // HashNote_Table* table = HashNote__deserialize(retrieved);
    // HashNote_Branch* branch = HashNote__get_branch(table, "hello");
    // printf("name: %s, first_note: %s\n", branch->name, branch->notes[0]->text);

    // free(retrieved);
    // HashNote__free_table(table);


    // ---------------------------

    // char command = parse_command(argc, argv[1]);

    // if(command == -1) {
    //     show_help();
    //     return 0;
    // }

    // switch(command) {
    //     case 'a':
    //         add(argc, argv); // arg 2 is the note to add
    //         break;
    //     case 's':
    //         show(argc, argv);
    //         break;
    //     case 'c':
    //         // complete(argc, argv);
    //         printf("Not Implemented\n");
    //         break;
    //     case 'r':
    //         printf("Not Implemented\n");
    //         break;
    //     default:
    //         printf("Error\n no command found");
    //         return 1;
    // }
}
