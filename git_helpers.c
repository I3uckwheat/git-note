#include <stdio.h>
#include <string.h>

#include <./git_helpers.h>

int get_branch_name(char *buffer, size_t bufferSize) {
    FILE *fp;
    fp = popen("git branch --show-current", "r");
    if(fp == NULL) {
        printf("Failed to access git");
        return 1;
    }

    while(fgets(buffer, bufferSize, fp) != NULL);
    buffer[strcspn(buffer, "\n")] = 0; // Removes the newline returned from the git command

    pclose(fp);
    return 0;
}

int get_dir_name(char *buffer, size_t bufferSize) {
    FILE *fp;
    fp = popen("git rev-parse --show-toplevel", "r");
    if(fp == NULL) {
        printf("Failed to access git");
        return 1;
    }

    while(fgets(buffer, bufferSize, fp) != NULL);
    pclose(fp);

    buffer[strcspn(buffer, "\n")] = 0; // Removes the newline returned from the git command
    strncpy(buffer, strrchr(buffer, '/') + 1, bufferSize); // Move the substring to the front of the buffer
    return 0;
}
