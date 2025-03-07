#include <./src/show.h>
#include <./src/file_helpers.h>

int show(int argc, char *argv[]) {
    FILE* noteFile = open_notes_file("r");
    if(noteFile == NULL) {
        printf("No notes exist for the current branch\n");
        return 1;
    }

    // TODO: If not formatted correctly, fix and update
    int line = 1;
    char fileLineBuffer[1000];
    while(fgets(fileLineBuffer, sizeof(fileLineBuffer), noteFile) != NULL) {
        printf("%d %s", line, fileLineBuffer);
        line++;
    };
    close_notes_file(noteFile);
    return 0;
}
