#include <stdio.h>
#include <stdlib.h>

#include <./complete.h>
#include <./file_helpers.h>

int read_file_into_memory(char *buffer, FILE *noteFile) {
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

    buffer = notes;
    return charactersRead;
}

int complete(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Not enough arguments to 'complete' command\n");
        exit(1);
    }

    int completedLine = atoi(argv[2]);

    FILE* noteFile = open_notes_file("r");
    if(noteFile == NULL) {
        printf("No notes exist for the current branch\n");
        return 1;
    }

    char *notes = {0};
    int characterCount = read_file_into_memory(notes, noteFile);
    close_notes_file(noteFile);

    FILE* noteFile = open_notes_file("w");
    if(noteFile == NULL) {
        printf("Failed to open notes for writing\n");
        return 1;
    }

    int lines;
    while(1) {
        // write line to file
        // after x lines, make complete
    }

    free(notes);
    return 0;
}
