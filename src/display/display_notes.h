#pragma once

#include "hashnote_table.h"

void Display__list_branches(const HashNote_Table* table); 
void Display__list_notes(const HashNote_Table* table, const char* branch_name);
void Display__note(const HashNote_Table* table, const char* branch_name, const size_t note_id);