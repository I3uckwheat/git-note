#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h> // Check on xcompat with mac
#include <errno.h>

#include "storage.h"
#include "file_helpers.h"


// TODO: Make configurable for where you want to store things

int Storage__build_root_directory(char* buffer, size_t buffer_size) {
    // TODO: Error check this if this fails
    return snprintf(buffer, buffer_size, "%s/%s", getenv("HOME"), ".git-note");
}

int Storage__verify_root_note_dir(char* root_note_directory) {
    // TODO: Error check this if this fails
    struct stat statBuffer;
    if(stat(root_note_directory, &statBuffer) != 0) { // If it doesn't exist, create the root dir
        int mkdir_result = mkdir(root_note_directory, 0755);
        if(mkdir_result != 0) {
            printf("%s: Unable to setup .notes directory", strerror(errno));
            return -1;
        }
    }

    return 0;
}

int Storage__build_note_file_path(char* buffer, size_t buffer_size, char* root_note_directory, char* repo_name) {
    // TODO: Error check this if this fails
    return snprintf(buffer, buffer_size, "%s/%s", root_note_directory, repo_name);
}

// TODO: Error handling
int Storage__store_serialized_table(char* serialized_table, char* repo_name) {
    char root_note_directory[4096 - 256];
    Storage__build_root_directory(root_note_directory, sizeof(root_note_directory));
    Storage__verify_root_note_dir(root_note_directory);

    char note_file_path[4096];
    Storage__build_note_file_path(note_file_path, sizeof(note_file_path), root_note_directory, repo_name);

    // TODO: figure out proper mode
    FILE* notes_file = fopen(note_file_path, "w");
    fputs(serialized_table, notes_file);
    fclose(notes_file);
    notes_file = NULL;

    return 0;
}

char* Storage__retrieve_serialized_table(char* repo_name) {
    char root_note_directory[4096 - 256];
    Storage__build_root_directory(root_note_directory, sizeof(root_note_directory));

    char note_file_path[4096];
    Storage__build_note_file_path(note_file_path, sizeof(note_file_path), root_note_directory, repo_name);

    FILE* notes_file = fopen(note_file_path, "r");
    return read_file_into_memory(notes_file);
    fclose(notes_file);
}