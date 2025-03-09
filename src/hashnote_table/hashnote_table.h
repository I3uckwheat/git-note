#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TABLE_CAPACITY 5000 // TODO: Figure out sane max
#define MAX_BRANCH_NAME_LENGTH 200
#define MAX_COMMENT_LENGTH 256
#define MAX_COMMENTS_ON_BRANCH 300

// Use this for refactoring maybe?
// typedef struct {
//     unsigned int index;
//     HashNote_Note* note;
// } HashNote_Note_Location;

// Forward declaration for HashNote_Note
typedef struct HashNote_Branch HashNote_Branch;

typedef struct {
    unsigned int id;
    char* text;
    HashNote_Branch* branch;
} HashNote_Note;

typedef struct HashNote_Branch {
    char* name; // TODO: Figure out maximum possible
    HashNote_Note** notes;
    int size;
    int count;
} HashNote_Branch;

typedef struct {
    HashNote_Branch** branches;
    int size;
    int count;
} HashNote_Table;

unsigned long HashNote__hash(char* branch_name);

HashNote_Table* HashNote__create_table();
HashNote_Branch* HashNote__create_branch(HashNote_Table* table, char* branch_name);
HashNote_Branch* HashNote_Table__upsert_branch(HashNote_Table* table, char* branch_name);
HashNote_Note* HashNote__create_note(HashNote_Table* table, char* branch_name, char* text);

HashNote_Branch* HashNote__get_branch(HashNote_Table* table, char* branch_name);
HashNote_Note* HashNote__get_note(HashNote_Table* table, char* branch_name, unsigned int id);

int HashNote_Table__delete_table(HashNote_Table* table);
int HashNote_Table__delete_Branch(HashNote_Table* table, char* branch_name);
int HashNote_Table__delete_note(HashNote_Table* table, char* branch_name, unsigned int id);

void HashNote__free_note(HashNote_Note* note);
void HashNote__free_branch(HashNote_Branch* branch);
void HashNote__free_table(HashNote_Table* table);

HashNote_Table* HashNote_deserialize();
char* HashNote__serialize_table(HashNote_Table* table);
char* HashNote__serialize_branch(HashNote_Branch* branch);