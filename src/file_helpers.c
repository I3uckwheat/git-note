#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "file_helpers.h"
#include "git_helpers.h"

#define MAX_FILE_PATH_LENGTH = 4096

//TODO: find a way to use 4096 more implicitly through a macro

char *read_file_into_memory(FILE *noteFile) {
    size_t chunkSize = 500;
    size_t chunks = 1;
    size_t charactersRead = 0;
    char *temp_notes;
    char *notes = malloc(sizeof(char) * chunkSize + 1);
    if(notes == NULL) {
        printf("Failed to complete note");
        exit(1);
    }

    while(1) {
        size_t read = fread(notes + charactersRead, sizeof(char), chunkSize, noteFile);
        if(read < chunkSize) {
            if(feof(noteFile)) {
                charactersRead += read;
                break;
            } else if (ferror(noteFile)) {
                printf("There was an error loading the note file\n");
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

    return notes;
}
