#pragma once

#include <stdio.h>
#include <string.h>

typedef struct Note {
    int isComplete;
    char *body;
} Note;

typedef struct NoteContainer {
    int size;
    Note **notes;
} NoteContainer;

Note * new_note();
int free_note(Note *note);

// returns a null terminated array
NoteContainer * parse_notes(FILE *noteFile);
int delete_notes(NoteContainer *notes);
