#pragma once

#include <string.h>
#include <ctype.h>
#include <stdio.h>

/// @brief copies string into buffer while removing whitespace
/// @param buffer - must be size of of input string or larger
/// @param string - null terminated string
/// @param length - length of string and buffer, including null terminator
/// @return 0 on success, -1 on fail
static inline int String__strntrim(char* buffer, char* string, size_t length) {
    char* start = string;
    char* end = string + length - 1;
    for(size_t i = 0; i < length; i++) {
        if(!isspace(*start)) {
            break;
        }

        start++; // moving starting char forward
    }

    for(size_t i = length; i != 0; i--) {
        if(!isspace(*end)) {
            break;
        }
        end--; // moving ending char backwards
    }

    if(end < start) {
        buffer[0] = '\0';
        return 0;
    }

    strncpy(buffer, start, end - start + 1);
    buffer[end - start + 1] = '\0';
    return 0;
}

static inline int String__parse_note_id_string(const char* input) {
    if(input == NULL) {
        printf("Must enter a note_id: git-note [branch] [note id]\n");
        exit(1);
    }

    const size_t note_id = (int)strtol(input, NULL, 10);

    if(note_id == 0) {
        printf("Invalid note id: %s\n", input);
        exit(1);
    }

    return note_id;
}

/// @brief Turns '\\n' into "\\n" respectively into a buffer string
/// @param buffer 
/// @param input null truncated string
/// @param size size of the buffer (should be double + 1 sizeof input string)
/// @return 
static inline int String__escape_newlines(char* buffer, const char* input, const size_t size) {
    size_t input_length = strlen(input);
    size_t buffer_location = 0;

    for(size_t i = 0; i < input_length; i++) {
        if(input[i] == '\n') {
            buffer[buffer_location] = '\\';
            buffer[buffer_location + 1] = 'n';
            buffer_location += 2;
        } else if(input[i] == '|') { // need to escape these for parsing later
            buffer[buffer_location] = '\\';
            buffer[buffer_location + 1] = '|';
            buffer_location += 2;
        } else {
            buffer[buffer_location] = input[i];
            buffer_location++;
        }

        if(buffer_location > size) {
            printf("Buffer is too small to escape into\n");
            exit(1);
        }
    }

    buffer[buffer_location] = '\0';
    return 0;
}

/// @brief Turns "\\n" into '\n' into a buffer string
/// @param buffer must be at least as big as the input string
/// @param input 
/// @param size must be the size of the input string
/// @return 
static inline int String__unescape_newlines_n(char* buffer, const char* input, const size_t input_size) {
    size_t buffer_location = 0;

    for(size_t i = 0; i < input_size; i++) {
        if(input[i] == '\\' && input[i + 1] == 'n') {
            i++; // skipping next iteration, which would be the `n` we already captured
            buffer[buffer_location] = '\n';
        }else if (input[i] == '\\' && input[i + 1] == '|') {
            i++;
            buffer[buffer_location] = '|';
        } else {
            buffer[buffer_location] = input[i];
        }

        buffer_location++;
    }

    buffer[buffer_location] = '\0';
    return 0;
}
