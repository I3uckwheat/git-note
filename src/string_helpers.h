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
