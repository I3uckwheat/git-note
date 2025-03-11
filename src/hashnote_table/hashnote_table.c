#include <time.h>

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
        for(size_t i = 0; i < branch->size; i++) {
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
        for(size_t i = 0; i < table->size; i++) {
            HashNote__free_branch(table->branches[i]);
            table->branches[i] = NULL;
        }

        free(table->branches);
    }

    free(table);
}

unsigned long HashNote__hash(const char* branch_name) {
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

HashNote_Branch* HashNote__create_branch(HashNote_Table* table, const char* branch_name) {
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
    for (size_t i = 0; i < branch->size; i++) {
        branch->notes[i] = NULL;
    }

    // Hash key and ensure within bounds
    unsigned long hash = HashNote__hash(branch->name) % table->size;
    table->branches[hash] = branch;
    table->count++;

    return branch;
}

HashNote_Branch* HashNote_Table__upsert_branch(HashNote_Table* table, const char* branch_name) {
    unsigned long hash = HashNote__hash(branch_name);
    HashNote_Branch* branch = table->branches[hash];
    if(branch != NULL) return branch;
    return HashNote__create_branch(table, branch_name);
}

HashNote_Note* HashNote__create_new_note(HashNote_Table* table, const char* branch_name, const char* text) {
    time_t created_at = time(NULL);
    return HashNote__create_note_on_table(table, branch_name, created_at, created_at, text);
}

HashNote_Note* HashNote__create_note_on_table(HashNote_Table* table, const char* branch_name, const time_t created_at, const time_t modified_at, const char* text) {
    HashNote_Branch* branch = HashNote_Table__upsert_branch(table, branch_name);
    if(branch->count == branch->size) return NULL;

    HashNote_Note* note = (HashNote_Note*) malloc(sizeof(HashNote_Note));
    note->text = (char*) malloc(MAX_COMMENT_LENGTH);
    strncpy(note->text, text, MAX_COMMENT_LENGTH - 1);
    note->text[MAX_COMMENT_LENGTH - 1] = '\0';

    branch->notes[branch->count] = note;
    note->id = branch->count;
    note->branch = branch;
    note->created_at = created_at;
    note->modified_at = modified_at;
    branch->count++;

    return note;
}

HashNote_Branch** HashNote__get_all_branches(const HashNote_Table* table) {
    HashNote_Branch** branch_ptr_array = calloc(table->count, sizeof(HashNote_Branch*));
    size_t branch_ptr_array_index = 0;
    
    for(size_t i = 0; i < table->size; i++) {
        HashNote_Branch* branch = table->branches[i];
        if(branch != NULL) {
            branch_ptr_array[branch_ptr_array_index] = branch;
            branch_ptr_array_index++;
        }

        if(branch_ptr_array_index == table->count) return branch_ptr_array;
    }

    return branch_ptr_array;
}

HashNote_Branch* HashNote__get_branch(const HashNote_Table* table, const char* branch_name) {
    unsigned long hash = HashNote__hash(branch_name);
    return table->branches[hash]; 
}

HashNote_Note* HashNote__get_note(const HashNote_Table* table, const char* branch_name, const unsigned int id) {
    HashNote_Branch* branch = HashNote__get_branch(table, branch_name);
    if(!branch) return NULL;

    for(size_t i = 0; i < branch->size; i++) {
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
    for(size_t i = 0; i < branch->size; i++) {
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

// v1
// branchname|created_at|modified_at|note|
// branchname2|created_at|modified_at|note2|
// branchname|created_at|modified_at|note3|
char* HashNote__serialize_table(HashNote_Table* table) {
    // TODO: find better sane default, this is expensive
    // It's also expensive to double it every time we need more, but may be OK
    // It's just allocating way too much memory for the average usecase
    size_t buffer_size = (table->count * MAX_BRANCH_NAME_LENGTH) + (table->count * MAX_COMMENT_LENGTH * 2);
    size_t buffer_offset = 0;
    char* serialized_table_buffer = calloc(1, buffer_size);
    serialized_table_buffer[0] = '\0';

    for(size_t i = 0; i < table->size; i++) {
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
    }

    return serialized_table_buffer;
}

// TODO: Handle escape sequences
// \n entered into a note will break the whole formatting
char* HashNote__serialize_branch(HashNote_Branch* branch) {
    if (!branch) return NULL;

    size_t total_size = MAX_BRANCH_NAME_LENGTH + (branch->size * MAX_COMMENT_LENGTH) + 1; // +1 for null terminator
    char* serialized_branch = (char*) malloc(total_size);
    if (serialized_branch == NULL) {
        fprintf(stderr, "Failed to allocate memory for serialization\n");
        return NULL;
    }

    serialized_branch[0] = '\0';  // Start with an empty string

    size_t offset = strlen(serialized_branch);
    
    for (size_t i = 0; i < branch->size; i++) {
        HashNote_Note* note = branch->notes[i];
        if (!note) continue;

        size_t remaining_space = total_size - offset - 1; // Leave space for null
        if(remaining_space <= 0) {
            // TODO: dynamic allocation for this case
            fprintf(stderr, "String size too small for notes");
            exit(1);
        }

        // Adding to the string based on the offset
        // branch||created_at|modified_at|note\n
        snprintf(serialized_branch + offset, MAX_COMMENT_LENGTH - 1, "%s|%lu|%lu|%s|\n", branch->name, note->created_at, note->modified_at, note->text);
        offset = strlen(serialized_branch);
        remaining_space = total_size - offset;
    }

    serialized_branch[total_size - 1] = '\0';  // Ensure null termination
    return serialized_branch;
}

void HashNote__deserialize_note(HashNote_Table* table, char* note_line_ptr) {
    char branch_name[256]; 
    memset(branch_name, '\0', sizeof(branch_name));
    time_t created_at = 0;
    time_t modified_at = 0;

    // Branch name
    char* branch_name_end_ptr = strchr(note_line_ptr, '|');
    strncpy(branch_name, note_line_ptr, branch_name_end_ptr - note_line_ptr);
    note_line_ptr = branch_name_end_ptr + 1; // increment pointer, and move past deliminator

    // created_at
    // FIXME: Error handle case where returns 0
    long int parsed_created_at = strtol(note_line_ptr, &note_line_ptr, 10);
    created_at = (time_t)parsed_created_at;
    note_line_ptr++; // move past deliminator

    // modified_at
    // FIXME: Error handle case where returns 0
    long int parsed_modified_at = strtol(note_line_ptr, &note_line_ptr, 10);
    modified_at = (time_t)parsed_modified_at;
    note_line_ptr++; // move past deliminator

    // note text
    char* note_text_end_ptr = strchr(note_line_ptr, '|');
    char* note_text = calloc(1, note_text_end_ptr - note_line_ptr);
    strncpy(note_text, note_line_ptr, note_text_end_ptr - note_line_ptr);

    HashNote__create_note_on_table(table, branch_name, created_at, modified_at, note_text);

    free(note_text);
}

// TODO: possible optimization is to do this while reading from the file by line instead of loading
// into memory first
HashNote_Table* HashNote__deserialize(char* hash_note_string) {
    HashNote_Table* table = HashNote__create_table();

    char* last_note_end = hash_note_string;
    char* current_char_ptr = hash_note_string;
    while(*current_char_ptr != '\0') {
        // read until newline, pass string to deserialize_note
        if(*current_char_ptr == '\n' && *(current_char_ptr + 1) != '\0') {
            HashNote__deserialize_note(table, last_note_end);
            last_note_end = current_char_ptr + 1; // moving past newline for next note
        }

        current_char_ptr++;
    }

    // Grab the last line if we don't end on a newline
    if(last_note_end < current_char_ptr) {
        HashNote__deserialize_note(table, last_note_end);
    }

    return table;
}
