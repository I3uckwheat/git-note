#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "complete.h"
#include "file_helpers.h"
#include "note.h"

char * write_to_delimiter(char *lineStart, char delim, FILE *dest) {
    // Write characters for the line between the start of the line, and the next \n
    int charCounter = 0;
    while(1) {
        int result = fputc(lineStart[charCounter], dest);
        if(result != lineStart[charCounter]) { // FIXME: use proper EOF
            printf("Failed to write to file\n"); // FIXME: prevent loss of whole file (use dupe)
            return 0;
        }

        if(lineStart[charCounter] == delim) break;
        charCounter++;
    }

    return lineStart + charCounter + 1;
}

int complete(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Not enough arguments to 'complete' command\n");
        exit(1);
    }

    int completedLine = atoi(argv[2]) - 1;

    FILE* noteFile = open_notes_file("r+");
    if(noteFile == NULL) {
        printf("No notes exist for the current branch\n");
        return 1;
    }

    NoteContainer *noteContainer = parse_notes(noteFile);
    if(noteContainer == NULL) {
        printf("Failed to parse notes\n");
        free(noteFile);
        exit(1);
    }

    if(noteContainer->size < completedLine && completedLine > 0) return 0;
    if(noteContainer->notes[completedLine]->isComplete == 1) {
        noteContainer->notes[completedLine]->isComplete = 0;
    } else {
        noteContainer->notes[completedLine]->isComplete = 1;
    }

    rewind(noteFile);

    for(int i = 0; i < noteContainer->size - 1; i++) {
        Note * currentNote = noteContainer->notes[i];
        printf("currentNote: %s, isComplete: %d, i: %d\n", currentNote->body, currentNote->isComplete, i);

        if(currentNote->isComplete == 1) {
            fputs("[x] |", noteFile);
        } else {
            fputs("[ ] |", noteFile);
        }

        fputs(currentNote->body, noteFile);
        fputs("\n", noteFile);
    }

    close_notes_file(noteFile);
    delete_notes(noteContainer);
    return 0;
}
