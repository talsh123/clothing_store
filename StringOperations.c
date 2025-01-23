#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "StringOperations.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char* trimwhitespace(char* str) {

    // Skip leading whitespace
    while (isspace((unsigned char)*str))
        str++;

    // Set the start of the string
    char* start = str;

    // Forward till the end of the string
    while (!isspace((unsigned char)*str))
        str++;
    char* end = str;

    // Calculate the length of the trimmed string and allocate memory
    size_t newLength = (end - start);

    char* newString = (char*)malloc(newLength); // +1 for null terminator
    strncpy(newString, start, newLength);
    newString[newLength] = '\0';
    return newString;
}

int compareStrings(const char* str1, const char* str2) {
    // Check if both strings are non-NULL
    if (str1 == NULL || str2 == NULL) {
        return 0; // NULL strings cannot be identical
    }

    // Use strcmp to compare the strings
    if (!strcmp(str1, str2)) {
        return 1; // Strings are identical
    }
    else {
        return 0; // Strings are different
    }
}