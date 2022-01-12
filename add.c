#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <./file_helpers.h>

int add(int argc, char *argv[]) {
    char *message = argv[2];
    FILE* noteFile = open_notes_file("a");

    // Failing to open the file, is likely due to a missing directory
    if (noteFile == NULL && errno == ENOENT) {
        if(create_note_dir_structure() != 0) {
            printf("%s: Failed to create note\n", strerror(errno));
            exit(1);
        }

        // Recurse to attempt to open file again
        return add(argc, argv);
    }

    fprintf(noteFile, "%s\n", message);
    close_notes_file(noteFile);
    return 0;
}