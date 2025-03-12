#pragma once

#include <stdio.h>

FILE *open_notes_file();
int close_notes_file(FILE *noteFile);
int create_note_dir_structure(); 
int get_note_path(char *buffer, size_t bufferSize);
char *read_file_into_memory(FILE *noteFile);
char* edit_with_git_editor(char* contents);
