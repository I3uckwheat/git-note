#include "hashnote_table.h"

void HashNote__free_note(HashNote_Note* note) {
    if(note == NULL) return;
    note->branch = NULL;
    free(note->text);
    free(note);
}

void HashNote__free_branch(HashNote_Branch* branch) {
    if(branch == NULL) return;
    if(branch->notes != NULL) {
        for(int i = 0; i < branch->size; i++) {
            HashNote__free_note(branch->notes[i]);
            branch->notes[i] = NULL;
            branch->count--;
            if(branch->count <= 0) break;
        }

        free(branch->notes);
        branch->notes = NULL;
    } 

    free(branch->name);
    free(branch);
}

void HashNote__free_table(HashNote_Table* table) {
    if(table == NULL) return;
    if(table->branches) {
        for(int i = 0; i < table->size; i++) {
            HashNote__free_branch(table->branches[i]);
            table->branches[i] = NULL;
            table->count--;
            if(table->count <= 0) break;
        }

        free(table->branches);
    }

    free(table);
}

unsigned long HashNote__hash(char* branch_name) {
    unsigned long hash = 5381;  // Initialize with a large prime number
    int c;

    while ((c = *branch_name++)) {  // Iterate through characters
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % TABLE_CAPACITY;  // Ensure it fits within array bounds
}

HashNote_Table* HashNote__create_table() {
    HashNote_Table* table = (HashNote_Table*) malloc(sizeof(HashNote_Table));
    if(!table) return NULL;

    table->size = TABLE_CAPACITY;
    table->count = 0;

    table->branches = (HashNote_Branch**) calloc(table->size, sizeof(HashNote_Branch*));
    if(!table->branches) return NULL;

    return table;
}

HashNote_Branch* HashNote__create_branch(HashNote_Table* table, char* branch_name) {
    if(!table) return NULL;
    // TODO: Grow table if out of size
    if (table->count == table->size) return NULL;

    HashNote_Branch* branch = (HashNote_Branch*) malloc(sizeof(HashNote_Branch));
    if (!branch) return NULL;

    branch->name = (char*) malloc(MAX_BRANCH_NAME_LENGTH);
    if (!branch->name) {
        free(branch);
        return NULL;
    }

    // Copy branch name safely
    strncpy(branch->name, branch_name, MAX_BRANCH_NAME_LENGTH - 1);
    branch->name[MAX_BRANCH_NAME_LENGTH - 1] = '\0';  // Ensure null termination

    // Allocate and initialize notes array
    branch->size = MAX_COMMENTS_ON_BRANCH;
    branch->count = 0;
    branch->notes = (HashNote_Note**) calloc(MAX_COMMENTS_ON_BRANCH, sizeof(HashNote_Note*));
    if (!branch->notes) {
        free(branch->name);
        free(branch);
        return NULL;
    }

    // Initialize notes array to NULL
    for (int i = 0; i < branch->size; i++) {
        branch->notes[i] = NULL;
    }

    // Hash key and ensure within bounds
    unsigned long hash = HashNote__hash(branch->name) % table->size;
    table->branches[hash] = branch;
    table->count++;

    return branch;
}

HashNote_Branch* HashNote_Table__upsert_branch(HashNote_Table* table, char* branch_name) {
    unsigned long hash = HashNote__hash(branch_name);
    HashNote_Branch* branch = table->branches[hash];
    if(branch != NULL) return branch;
    return HashNote__create_branch(table, branch_name);
}

HashNote_Note* HashNote__create_note(HashNote_Table* table, char* branch_name, char* text) {
    HashNote_Branch* branch = HashNote_Table__upsert_branch(table, branch_name);
    if(branch->count == branch->size) return NULL;

    HashNote_Note* note = (HashNote_Note*) malloc(sizeof(HashNote_Note));
    note->text = (char*) malloc(MAX_COMMENT_LENGTH);
    strncpy(note->text, text, MAX_COMMENT_LENGTH - 1);
    note->text[MAX_COMMENT_LENGTH - 1] = '\0';

    branch->notes[branch->count] = note;
    note->id = branch->count;
    note->branch = branch;
    branch->count++;

    return note;
}

HashNote_Branch* HashNote__get_branch(HashNote_Table* table, char* branch_name) {
    unsigned long hash = HashNote__hash(branch_name);
    return table->branches[hash]; 
}

HashNote_Note* HashNote__get_note(HashNote_Table* table, char* branch_name, unsigned int id) {
    HashNote_Branch* branch = HashNote__get_branch(table, branch_name);
    if(!branch) return NULL;

    for(int i = 0; i < branch->size; i++) {
        HashNote_Note* note = branch->notes[i];
        if(!note) continue;
        if(note->id == id) return note;
    }

    return NULL;
}

int HashNote_Table__delete_table(HashNote_Table* table) {
    HashNote__free_table(table);
    return 0;
}

int HashNote_Table__delete_Branch(HashNote_Table* table, char* branch_name) {
    unsigned long hash = HashNote__hash(branch_name);
    HashNote_Branch* branch = HashNote__get_branch(table, branch_name);

    if(!branch) return 0;
    HashNote__free_branch(branch);
    table->branches[hash] = NULL;
    return 0;
}

int HashNote_Table__delete_note(HashNote_Table* table, char* branch_name, unsigned int id) {
    HashNote_Note* note = HashNote__get_note(table, branch_name, id);
    HashNote_Branch* branch = note->branch;
    if(!note) return 0;

    unsigned int index = 0;
    for(int i = 0; i < branch->size; i++) {
        HashNote_Note* note = branch->notes[i];
        if(!note) continue;
        if(note->id == id) {
            index = i;
            break;
        }
    }

    HashNote__free_note(note);
    branch->notes[index] = NULL;
    return 0;
}