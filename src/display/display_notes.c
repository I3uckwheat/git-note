#include <time.h>
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

void Display__note(const HashNote_Table* table, const char* branch_name, const size_t note_id) {
    const HashNote_Note* note = HashNote__get_note(table, branch_name, note_id);
    if(!note) {
        printf("No note found on branch '%s' with id '%lu'. Check with `git-note -l`.\n", branch_name, note_id);
        exit(1);
    }

    char created_at[26];
    char modified_at[26];
    ctime_r(&note->created_at, created_at);
    ctime_r(&note->modified_at, modified_at);

    // ctime ends with a newline, this strips it out
    created_at[strlen(created_at) - 1] = '\0';
    modified_at[strlen(modified_at) - 1] = '\0';

    printf("id: %lu | Created: %s | Modified: %s\n", note->id, created_at, modified_at);
    printf("----------------------------------------------------------------------------\n");
    printf("%s\n", note->text);
}