#include <stdio.h>
#include <string.h>

#include "git_helpers.h"

int GitHelpers__get_branch_name(char *buffer, size_t bufferSize) {
    FILE *fp;
    fp = popen("git branch --show-current", "r");
    if(fp == NULL) {
        printf("Failed to access git");
        return 1;
    }

    while(fgets(buffer, bufferSize, fp) != NULL);
    buffer[strcspn(buffer, "\n")] = 0; // Removes the newline returned from the git command

    // Converts all '/' into another delimiter
    size_t i = 0;
    while(buffer[i] != '\0' || i != bufferSize) {
        if(buffer[i] == '/') buffer[i] = ':';
        i++;
    }

    pclose(fp);
    return 0;
}

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

int get_configured_editor_path(char *buffer, size_t bufferSize) {
    FILE *fp;

    char editor_path[4028];
    memset(editor_path, '\0', sizeof(editor_path));

    fp = popen("echo $(git config --get core.editor)", "r");
    if(fp == NULL) {
        printf("Failed to access git, is 'git' installed?");
        return 1;
    }
    while(fgets(editor_path, sizeof(editor_path), fp) != NULL);
    buffer[strcspn(editor_path, "\n")] = 0; // Removes the newline returned from the git command

    if(editor_path[0] == '\0') {
        printf("core.editor is not configured in git.\n");
        printf("run `git config --global core.editor vim` to configure.\n");
        return 1;
    }
    pclose(fp);

    fp = popen("which $(git config --get core.editor)", "r");
    while(fgets(buffer, bufferSize, fp) != NULL);
    buffer[strcspn(buffer, "\n")] = 0; // Removes the newline returned from the git command
    pclose(fp);

    return 0;
}