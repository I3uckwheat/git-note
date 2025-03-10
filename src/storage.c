#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h> // Check on xcompat with mac
#include <errno.h>

#include "storage.h"

int Storage__store_serialized_table(char* serialized_table, char* repo_name) {
    char root_note_directory[4096 - 256];

    // TODO: Error check this if this fails
    int success = snprintf(root_note_directory, sizeof(root_note_directory), "%s/%s", getenv("HOME"), ".git-note");

    struct stat statBuffer;
    if(stat(root_note_directory, &statBuffer) != 0) { // If it doesn't exist, create the root dir
        int mkdir_result = mkdir(root_note_directory, 0755);
        if(mkdir_result != 0) {
            printf("%s: Unable to setup .notes directory", strerror(errno));
            return -1;
        }
    }

    char note_file_path[4096];
    snprintf(note_file_path, sizeof(note_file_path), "%s/%s", root_note_directory, repo_name);

    // TODO: figure out proper mode
    FILE* notes_file = fopen(note_file_path, "w");
    fputs(serialized_table, notes_file);
    fclose(notes_file);
    notes_file = NULL;

    return 0;
}