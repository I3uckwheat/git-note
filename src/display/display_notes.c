#include "display_notes.h"
#include "hashnote_table.h"

// FIXME: return an int for success/fail
void Display__list_branches(const HashNote_Table* table) { 
    HashNote_Branch** branches = HashNote__get_all_branches(table);

    printf("| branch (notes count) : last note\n");
    printf("----------------------------------\n");
    for(size_t i = 0; i < table->count; i++) {
        HashNote_Branch* branch = branches[i];
        printf("| %s (%li): %s\n", branch->name, branch->count, branch->notes[0]->text);
    }
}

// FIXME: return an int for success/fail
void Display__list_notes(const HashNote_Table* table, const char* branch_name) {
    HashNote_Branch* branch = HashNote__get_branch(table, branch_name);
    if(!branch) {
        printf("No notes found for branch: %s\n", branch_name);
        return;
    }

    printf("id | note\n");
    printf("-----------\n");
    for(size_t i = 0; i < branch->count; i++) {
        HashNote_Note* note = branch->notes[i];
        printf(" %li | %s\n", note->id, note->text);
    }
}
