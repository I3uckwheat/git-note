#include "display_notes.h"
#include "hashnote_table.h"

void Display__list_branches(const HashNote_Table* table) { 
    HashNote_Branch** branches = HashNote__get_all_branches(table);

    printf("| branch (notes count) : last note\n");
    printf("----------------------------------\n");
    for(size_t i = 0; i < table->count; i++) {
        HashNote_Branch* branch = branches[i];

        char* note_text = strdup(branch->notes[0]->text);
        if(!note_text) {
            printf("Failed to dupe string\n");
            exit(1);
        }

        char* rest = note_text;
        char* line = strtok_r(rest, "\n", &rest);
        printf("| %s (%li): %s\n", branch->name, branch->count, line);

        while((line = strtok_r(rest, "\n", &rest))) {
            printf("  %s\n", line);
        }


        free(note_text);
    }
}

void Display__list_notes(const HashNote_Table* table, const char* branch_name) {
    HashNote_Branch* branch = HashNote__get_branch(table, branch_name);
    if(!branch) {
        printf("No notes found for branch: %s\n", branch_name);
        return;
    }

    printf("id | note\n");
    printf("---------\n");
    for(size_t i = 0; i < branch->count; i++) {
        HashNote_Note* note = branch->notes[i];

        char* text_copy = strdup(note->text);
        if(!text_copy) {
            printf("Failed to dupe string\n");
            exit(1);
        }

        char* rest = text_copy;
        char* line = strtok_r(rest, "\n", &rest);
        printf(" %li | %s\n", note->id, line);

        while((line = strtok_r(rest, "\n", &rest))) {
            printf("     %s\n", line);
        }

        free(text_copy);
    }
}

// void Display__show_note() {

// }
