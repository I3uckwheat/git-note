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

// branch||comment|comment|comment|\n
// branch2||comment|comment|comment|\n
// branch3||comment|comment|comment|\n
char* HashNote__serialize_table(HashNote_Table* table) {
    // TODO: find better sane default, this is expensive
    // It's also expensive to double it every time we need more, but may be OK
    // It's just allocating way too much memory for the average usecase
    size_t buffer_size = (table->count * MAX_BRANCH_NAME_LENGTH) + (table->count * MAX_COMMENT_LENGTH * 2);
    size_t buffer_offset = 0;
    char* serialized_table_buffer = calloc(1, buffer_size);
    serialized_table_buffer[0] = '\0';

    for(int i = 0; i < table->size; i++) {
        HashNote_Branch* branch = table->branches[i];
        if(!branch) continue;

        size_t size_remaining = buffer_size - buffer_offset;

        char* serialized_branch = HashNote__serialize_branch(branch);
        size_t serialized_branch_size = strlen(serialized_branch);

        if(serialized_branch_size > size_remaining) {
            size_t new_buffer_size = buffer_size * 2;
            serialized_table_buffer = realloc(serialized_table_buffer, new_buffer_size);
            if(!serialized_table_buffer) {
                fprintf(stderr, "Failed to allocate memory for serialization\n");
                return NULL;
            }
        }

        strncat(serialized_table_buffer, serialized_branch, serialized_branch_size);
        free(serialized_branch);
        buffer_offset += serialized_branch_size + 1;
        serialized_table_buffer[buffer_offset - 1] = '\n';
    }

    return serialized_table_buffer;
}

// FIXME: This likely has memory issues
// branch||comment|comment|comment|\n
char* HashNote__serialize_branch(HashNote_Branch* branch) {
    if (!branch) return NULL;

    // TODO: This uses a lot of memory unnecessarily
    // Using resizing of strings instead of just setting them based on a max
    size_t total_size = MAX_BRANCH_NAME_LENGTH + (branch->size * MAX_COMMENT_LENGTH) + 1; // +1 for null terminator
    char* serialized_branch = (char*) malloc(total_size);
    if (serialized_branch == NULL) {
        fprintf(stderr, "Failed to allocate memory for serialization\n");
        return NULL;
    }

    serialized_branch[0] = '\0';  // Start with an empty string
    snprintf(serialized_branch, MAX_BRANCH_NAME_LENGTH - 1, "%s||", branch->name);

    size_t offset = strlen(serialized_branch);
    
    for (int i = 0; i < branch->size; i++) {
        HashNote_Note* note = branch->notes[i];
        if (!note) continue;

        size_t remaining_space = total_size - offset - 1; // Leave space for null
        if(remaining_space <= 0) {
            fprintf(stderr, "String size too small for notes");
            exit(1);
        }

        // Adding to the string based on the offset
        snprintf(serialized_branch + offset, MAX_COMMENT_LENGTH - 1, "%s|", note->text);
        offset = strlen(serialized_branch);
        remaining_space = total_size - offset;
    }

    serialized_branch[total_size - 1] = '\0';  // Ensure null termination
    return serialized_branch;
}

// branch||comment|comment|comment|\n
// branch2||comment|comment|comment|\n
// branch3||comment|comment|comment|\n
HashNote_Table* HashNote__deserialize(char* hash_note_string, size_t length) {
    HashNote_Table* table = HashNote__create_table();
    char* current_branch_name = calloc(1, MAX_BRANCH_NAME_LENGTH);
    HashNote_Branch* current_branch = NULL;

    size_t offset = 0;
    for(size_t i = 0; i < length; i++) {
        char character = hash_note_string[i];
        if(character == '\0') break;
        if(character == '\n') {
            memset(current_branch_name, 0, MAX_BRANCH_NAME_LENGTH); // Zero the string
            current_branch = NULL;
            offset++;
        };

        if(character == '|' && hash_note_string[i + 1] == '|') {
            strncpy(current_branch_name, hash_note_string + offset, i - offset);
            current_branch = HashNote__create_branch(table, current_branch_name);
            offset = i + 1; // To offset the second '|' match
            i = i + 1; // skip the next pipe in iteration
        }

        if(character == '|') {
            if(i - offset != 0) {
                char* note_str = calloc(1, MAX_COMMENT_LENGTH);
                strncpy(note_str, hash_note_string + offset, i - offset);
                HashNote_Note* note = HashNote__create_note(table, current_branch_name, note_str);
                free(note_str); // Do I need this? I think so, check with Valgrind
            }

            offset = i + 1;
        }
    }

    free(current_branch_name);
    return table;
}
