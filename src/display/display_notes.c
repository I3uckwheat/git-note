#include "display_notes.h"
#include "hashnote_table.h"

void Display__list_branches(HashNote_Table* table) { 
    HashNote_Branch** branches = HashNote__get_all_branches(table);

    printf("| branch (notes count) : last note\n----------------------------------\n");
    for(size_t i = 0; i < table->count; i++) {
        HashNote_Branch* branch = branches[i];
        printf("| %s (%li): %s\n", branch->name, branch->count, branch->notes[0]->text);
    }
}