#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include<sys/types.h>
#include <unistd.h> // Linux only, mac?
#include <sys/wait.h>

#include "file_helpers.h"
#include "git_helpers.h"
#include "string_helpers.h"

#define MAX_FILE_PATH_LENGTH = 4096

//TODO: find a way to use 4096 more implicitly through a macro

char *read_file_into_memory(FILE *noteFile) {
    size_t chunkSize = 500;
    size_t chunks = 1;
    size_t charactersRead = 0;
    char *temp_notes;
    char *notes = malloc(sizeof(char) * chunkSize + 1);
    if(notes == NULL) {
        printf("Failed to allocate note memory");
        exit(1);
    }

    while(1) {
        size_t read = fread(notes + charactersRead, sizeof(char), chunkSize, noteFile);
        if(read < chunkSize) {
            if(feof(noteFile)) {
                charactersRead += read;
                break;
            } else if (ferror(noteFile)) {
                printf("There was an error loading the file\n");
                exit(1);
            }
        }

        chunks++;
        charactersRead += read;

        temp_notes = realloc(notes, chunkSize * chunks);
        if(temp_notes == NULL) {
            printf("Failed to allocate memory\n");
            exit(1);
        }

        notes = temp_notes;
    } 

    notes[charactersRead] = '\0';
    return notes;
}

// TODO: Find a better place for this
// Maybe use FILE as a return?
// TODO: Handle errors properly
char* edit_with_git_editor(char* content) {
    FILE* tmp_edit_file = fopen("/tmp/git-note-tmp", "w");
    fputs(content, tmp_edit_file);
    fclose(tmp_edit_file);

    char editor_path[4096];
    int err = get_configured_editor_path(editor_path, sizeof(editor_path));
    if(err) {
        printf("FAILED TO FIND EDITOR\n");
        exit(1);
    }
    pid_t pid = fork();
    if(pid == 0) {
        char *exec_args[] = {editor_path, "/tmp/git-note-tmp", NULL};
        execvp(editor_path, exec_args);
    } else if (pid != 0) {
        wait(NULL);
    }

    // reopen for reading changes
    tmp_edit_file = fopen("/tmp/git-note-tmp", "r");
    char* results = read_file_into_memory(tmp_edit_file);
    fclose(tmp_edit_file);

    size_t len = strlen(results);
    char* trimmed_results = calloc(1, len);
    String__strntrim(trimmed_results, results, len);

    free(results);
    return trimmed_results;
}
