#include <./file_helpers.h>
#include <stdio.h>

int add(int argc, char *argv[]) {
    char *message = argv[2];
    FILE* noteFile = open_notes_file("a");
    fprintf(noteFile, "%s\n", message);

    close_notes_file(noteFile);
}