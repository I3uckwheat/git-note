#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include "add.h"
#include "show.h"
#include "complete.h"
#include "note.h"
#include "hashnote_table.h"

// struct command {
//     char* name;
//     char commandFlag;
// };

// static const struct command commands[] = {
//     {"show", 's'},
//     {"add", 'a'},
//     {"complete", 'c'},
//     {"remove", 'r'},
//     {"rm", 'r'},
// };

// void show_help() {
//     printf("Help\n");
// }

// // Returns -1 if invalid, commandFlag if valid
// char parse_command(int count, char *command) {
//     // If only one command is sent we have to assume the user wants to show the notes 
//     if(count == 1) { 
//         return 's';
//     }

//     if(count < 2) {
//         return -1;
//     }

//     for(int i = 0; i < sizeof commands / sizeof *commands; i++) {
//         char commandFlag = commands[i].commandFlag;

//         if(
//             strncmp(command, commands[i].name, 10) == 0 ||
//             (strlen(command) == 1 && command[0] == commandFlag)
//         ) {
//             return commandFlag;
//         }
//     }

//     return -1;
// }

int main(int argc, char *argv[]) {
    HashNote_Table* table = HashNote__create_table();

    HashNote__create_note(table, "hello", "world");
    HashNote__create_note(table, "hello", "world2j");
    HashNote__create_note(table, "hello", "world3");
    HashNote__create_note(table, "hello", "world4");
    HashNote__create_note(table, "foo", "bar");
    HashNote__create_note(table, "foo", "baz");
    HashNote__create_note(table, "foo", "fee");
    HashNote_Branch* branch = HashNote__get_branch(table, "hello");
    HashNote_Branch* branch2 = HashNote__get_branch(table, "foo");
    printf("\n\nbranch: %s, text of: %s\n", branch->name, branch->notes[branch->count - 2]->text);
    printf("branch: %s, text of: %s\n\n", branch2->name, branch2->notes[branch2->count - 2]->text);

    HashNote_Table__delete_note(table, "hello", 2);

    HashNote_Table__delete_Branch(table, "hello");
    HashNote_Table__delete_Branch(table, "foo");
    HashNote_Table__delete_table(table);

    // HashNote_Table__delete_Branch(table, "foo");
    // branch2 = HashNote__get_branch(table, "foo");
    // HashNote_Table__upsert(table, "hello2", "world2");
    // HashNote_Table__upsert(table, "how", "areyou");
    // HashNote_Table__print(table);
    // HashNote_Item* note1 = HashNote_Table__get_item(table, "hello");
    // if(note1 != NULL) {
    //     printf("branch: %s, comment: %s\n\n", note1->branch, note1->comment);
    // }
    // char* noteLineStart = "[ ] | hello world\n";
    // size_t noteSize = 19;

    // Note *test = new_note(noteLineStart, noteSize);
    // printf("Note: %s\n\n", test->body);
    // exit(0);
    // return 0;

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
