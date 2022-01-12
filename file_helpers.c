#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <./file_helpers.h>
#include <./git_helpers.h>

int get_note_directory(char *buffer, size_t bufferSize) {
    char projectDirName[256];
    get_dir_name(projectDirName, sizeof(projectDirName));

    snprintf(buffer, bufferSize, "%s/%s/%s", getenv("HOME"), ".notes", projectDirName);
}

FILE *open_notes_file(char *mode) {
    // Git branch lengths are limited to 255 characters
    char branchName[256];
    get_branch_name(branchName, sizeof(branchName));

    char noteDirectoryPath[4096 - 256];
    get_note_directory(noteDirectoryPath, sizeof(noteDirectoryPath));

    char noteFilePath[4096];
    snprintf(noteFilePath, sizeof(noteFilePath), "%s/%s", noteDirectoryPath, branchName);

    return fopen(noteFilePath, mode);
}

int close_notes_file(FILE *noteFile) {
	return fclose(noteFile);
}

int create_note_dir_structure() {
    char main_note_directory[4096];
    snprintf(main_note_directory, sizeof(main_note_directory), "%s/%s", getenv("HOME"), ".notes");

    struct stat statBuffer;
    if(stat(main_note_directory, &statBuffer) != 0) {
        if(mkdir(main_note_directory, 0755) != 0) {
            printf("%s: Unable to setup .notes directory", strerror(errno));
        }
    }

    char noteDirectoryPath[4096 - 256];
    get_note_directory(noteDirectoryPath, sizeof(noteDirectoryPath));

    return mkdir(noteDirectoryPath, 0775);
}