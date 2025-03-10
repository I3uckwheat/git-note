#include <stdio.h>
#include <string.h>

#include "git_helpers.h"

// int get_branch_name(char *buffer, size_t bufferSize) {
//     FILE *fp;
//     fp = popen("git branch --show-current", "r");
//     if(fp == NULL) {
//         printf("Failed to access git");
//         return 1;
//     }

//     while(fgets(buffer, bufferSize, fp) != NULL);
//     buffer[strcspn(buffer, "\n")] = 0; // Removes the newline returned from the git command

//     // Converts all '/' into another delimiter
//     int i = 0;
//     while(buffer[i] != '\0' || i != bufferSize) {
//         if(buffer[i] == '/') buffer[i] = ':';
//         i++;
//     }

//     pclose(fp);
//     return 0;
// }

int GitHelpers__get_dir_name(char *buffer, size_t bufferSize) {
    FILE *fp;
    fp = popen("git rev-parse --show-toplevel", "r");
    if(fp == NULL) {
        printf("Failed to access git");
        return 1;
    }

    while(fgets(buffer, bufferSize, fp) != NULL);
    pclose(fp);

    buffer[strcspn(buffer, "\n")] = 0; // Removes the newline returned from the git command
    memcpy(buffer, strrchr(buffer, '/') + 1, bufferSize); // Move the substring to the front of the buffer
    return 0;
}

// int get_configured_editor_path(char *buffer, size_t bufferSize) {
//     FILE *fp;
//     fp = popen("whereis $(git config --get core.editor)", "r");
//     if(fp == NULL) {
//         printf("Failed to access git");
//         return 1;
//     }

//     while(fgets(buffer, bufferSize, fp) != NULL);
//     buffer[strcspn(buffer, "\n")] = 0; // Removes the newline returned from the git command

//     pclose(fp);
//     return 0;
// }