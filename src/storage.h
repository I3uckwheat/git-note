#pragma once

// Config stored at ~/.git-note/config // TODO: Make configs
// notes stored with ~/.git-note/projects/notes

int Storage__store_serialized_table(char* serialized_table, char* repo_name);
char* Storage__retrieve_serialized_table(char* repo_name);