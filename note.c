#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <./note.h>
#include <./file_helpers.h>

Note * new_note(char *noteLineStart, size_t noteSize) {
    Note *newNote = malloc(sizeof(Note));
    newNote->body = malloc(sizeof(char) * noteSize + 1);

    if(newNote->body == NULL) {
        printf("Failed to allocate memory\n");
        exit(1);
    }

    if(noteSize < 5) {
        return NULL; // TODO: parse as string
    }

    if(strncmp(noteLineStart, "[ ]", 3) == 0) {
        newNote->isComplete = 0;
    } else if (strncmp(noteLineStart, "[x]", 3)  == 0) {
        newNote->isComplete = 1;
    } else {
        newNote->isComplete = 0;
    }

    char *noteStart = memchr(noteLineStart, '|', noteSize) + 1; // stripping out the '|';
    if(noteStart != NULL) {
        // Copy from '|' forward
        size_t noteSpan = strcspn(noteStart, "\n");
        memcpy(newNote->body, noteStart, noteSpan);
    } else {
        // Copy whole line
        memcpy(newNote->body, noteLineStart, noteSize);
    }

    newNote->body[noteSize] = '\0'; // ensuring null termination
    return newNote;
}

int free_note(Note *note) {
    if(note == NULL) return 0;
    if(note->body != NULL) free(note->body);
    free(note);
    return 0;
}

NoteContainer * parse_notes(FILE *noteFile) {
    NoteContainer *noteContainer = malloc(sizeof(NoteContainer));
    noteContainer->size = 1;
    noteContainer->notes = malloc(sizeof(Note));

    char * noteString = read_file_into_memory(noteFile);
    char * lineStart = noteString;
    size_t lineSpan = strcspn(lineStart, "\n");

    while(1) {
        Note * newNote = new_note(lineStart, lineSpan);
        noteContainer->notes[noteContainer->size - 1] = newNote;


        Note **moreNotes;
        noteContainer->size++;
        moreNotes = realloc(noteContainer->notes, sizeof(Note) * noteContainer->size + 1);
        if(moreNotes == NULL) {
            free(noteContainer->notes);
            free(noteContainer);
            printf("Failed to make more notes\n");
            exit(1);
        }
        noteContainer->notes = moreNotes;
        if(lineStart[lineSpan] == '\0') break;
        lineStart = &lineStart[lineSpan + 1];
    }

    free(noteString);
    return noteContainer;
};

int delete_notes(NoteContainer *noteContainer) {
    if(noteContainer == NULL) return 0;
    for(size_t i = 0; i < noteContainer->size; i++) {
        free_note(noteContainer->notes[i]);
    }

    if(noteContainer->notes != NULL) free(noteContainer->notes);
    free(noteContainer);
    return 0;
};