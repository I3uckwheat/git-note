#include <stdio.h>
#include <getopt.h>
#include <string.h>

struct command {
    char* name;
    char commandFlag;
};

static const struct command const commands[] = {
    {"add", 'a'},
    {"complete", 'c'},
    {"remove", 'r'},
    {"rm", 'r'},
};

void show_help() {
    printf("Help\n");
}

// Returns -1 if invalid, commandFlag if valid
char parse_command(int count, char *command) {
    if(count < 2) {
        return -1;
    }

    for(int i = 0; i < sizeof commands / sizeof *commands; i++) {
        char commandFlag = commands[i].commandFlag;

        if(
            strcmp(command, commands[i].name) == 0 ||
            (strlen(command) == 1 && command[0] == commandFlag)
        ) {
            return commandFlag;
        }
    }

    return -1;
}

int main(int argc, char *argv[]) {
    char command = parse_command(argc, argv[1]);

    if(command == -1) {
        show_help();
        return 1;
    }

    switch(command) {
        case 'a':
        case 'c':
        case 'r':
            printf("Not Implemented\n");
            break;
        default:
            printf("Error\n no command found");
    }

    printf("Welcome to git-note!\n");
}
