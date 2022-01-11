#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Move to helper file
void getBranchName(char *buffer, size_t buffer_size) {
    FILE *fp;
    fp = popen("git branch --show-current", "r");
    if(fp == NULL) {
        printf("Failed to access git");
    }

    while(fgets(buffer, buffer_size, fp) != NULL);
    buffer[strcspn(buffer, "\n")] = 0; // Removes the newline returned from the git command

    pclose(fp);
}

void add(int argc, char *argv[]) {
    char *message = argv[2];

    // Git branch lengths are limited to 255 characters
    char branchName[256];
    getBranchName(branchName, 256);

    // Check if .notes directory exists
    //  if not, create
    // check if .notes/branchname exists
    //  if not, create
    // write new line to ./notes/branchname

    printf("%s, %s\n", branchName, message);
}