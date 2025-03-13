#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TABLE_CAPACITY 500 // TODO: Figure out sane max
#define MAX_BRANCH_NAME_LENGTH 200
#define MAX_COMMENT_LENGTH 500
#define MAX_COMMENTS_ON_BRANCH 300


// Forward declaration for HashNote_Note
typedef struct HashNote_Branch HashNote_Branch;
typedef struct HashNote_Note HashNote_Note;

typedef struct HashNote_Note {
    size_t id;
    char* text;
    time_t created_at;
    time_t modified_at;
    HashNote_Branch* branch;
    HashNote_Note* next;
    HashNote_Note* prev;
} HashNote_Note;

typedef struct HashNote_Branch {
    char* name; // TODO: Figure out maximum possible
    HashNote_Note* head_note;
    size_t size;
    size_t count;
} HashNote_Branch;

typedef struct {
    HashNote_Branch** branches;
    size_t size;
    size_t count;
} HashNote_Table;

unsigned long HashNote__hash(const char* branch_name);

HashNote_Table* HashNote__create_table();
HashNote_Branch* HashNote__create_branch(HashNote_Table* table, const char* branch_name);
HashNote_Branch* HashNote_Table__upsert_branch(HashNote_Table* table, const char* branch_name);

HashNote_Note* HashNote__create_new_note(HashNote_Table* table, const char* branch_name, const char* text);
HashNote_Note* HashNote__create_note_on_branch_head(HashNote_Table* table, const char* branch_name, const time_t created_at, const time_t modified_at, const char* text);

HashNote_Branch** HashNote__get_all_branches(const HashNote_Table* table);
HashNote_Branch* HashNote__get_branch(const HashNote_Table* table, const char* branch_name);
HashNote_Note* HashNote__get_note(const HashNote_Table* table, const char* branch_name, const unsigned int id);

int HashNote_Table__delete_table(HashNote_Table* table);
int HashNote_Table__delete_Branch(HashNote_Table* table, char* branch_name);
int HashNote_Table__delete_note(const HashNote_Table* table, const char* branch_name, const unsigned int id);

void HashNote__free_note(HashNote_Note* note);
void HashNote__free_branch(HashNote_Branch* branch);
void HashNote__free_table(HashNote_Table* table);

HashNote_Table* HashNote__deserialize(char* hash_note_string);
char* HashNote__serialize_table(HashNote_Table* table);
char* HashNote__serialize_branch(HashNote_Branch* branch);
