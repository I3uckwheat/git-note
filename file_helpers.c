#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <./file_helpers.h>
#include <./git_helpers.h>

FILE *open_notes_file(char *mode) {
    // Git branch lengths are limited to 255 characters
    char branchName[256];
    get_branch_name(branchName, 256);

    // 256 reserved for git branch and \0
    char noteDirPath[4096 - 256];
    // TODO: Add project name
    snprintf(noteDirPath, sizeof(noteDirPath), "%s/%s", getenv("HOME"), ".notes");

    char notePath[4096];
    snprintf(notePath, sizeof(notePath), "%s/%s", noteDirPath, branchName);

    FILE *note = fopen(notePath, mode);
    // Failing to open the file, is likely due to a missing directory
    if (note == NULL && errno == ENOENT) {
        if (mkdir(noteDirPath, 0775) != 0) {
            printf("Failed to create note path\n");
            exit(1);
        }

        // Recurse to attempt to open file again
        return open_notes_file(mode);
    } else if (note == NULL) {
        printf("%s: Failed to open notes file\n", strerror(errno));
    }

    return note;
}

void close_notes_file(FILE *noteFile) {
	fclose(noteFile);
}