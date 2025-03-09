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
    char* serialized_notes = "hello||1one|2two|3three|4four|\nfoo||bar|baz|not a|weenie|\n";
    HashNote_Table* table = HashNote__deserialize(serialized_notes, strlen(serialized_notes));

    HashNote_Branch* branch = HashNote__get_branch(table, "foo");
    if(branch) {
        printf("First result note: %s\n", branch->notes[0]->text);
    } else{
        printf("Failed to make foo branch\n");
    }

    HashNote__free_table(table);

    // HashNote_Table* table = HashNote__create_table();

    // HashNote__create_note(table, "hello", "one");
    // HashNote__create_note(table, "hello", "two");
    // HashNote__create_note(table, "hello", "three");
    // HashNote__create_note(table, "hello", "four");

    // HashNote__create_note(table, "foo", "bar");
    // HashNote__create_note(table, "foo", "baz");
    // HashNote__create_note(table, "foo", "not a");
    // HashNote__create_note(table, "foo", "weenie");

    // HashNote_Branch* branch = HashNote__get_branch(table, "hello");

    // char* serialized = HashNote__serialize_table(table);

    // printf("%s", serialized);
    // free(serialized);
    // HashNote__free_table(table);

    // ---------------------------

    // HashNote_Table__delete_note(table, "hello", 2);
    // HashNote_Table__delete_Branch(table, "hello");
    // HashNote_Table__delete_Branch(table, "foo");
    // HashNote_Table__delete_table(table);


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
