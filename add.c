#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <./file_helpers.h>
#include <./git_helpers.h>

int open_editor() {
    char note_path[4096];
    get_note_path(note_path, sizeof(note_path));
    
    char configured_editor_path[4096];
    get_configured_editor_path(configured_editor_path, sizeof(configured_editor_path));

    char *arg_list[] = { "vim", note_path, NULL };
    execv(configured_editor_path, arg_list);
    return 0;
}

int add(int argc, char *argv[]) {
    char *message = argv[2];
    if(message == NULL) {
        return open_editor();
    }

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