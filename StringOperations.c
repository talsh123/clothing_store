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

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void clrscr()
{
    system("@cls||clear");
}

// Function to validate the date format "DD-MM-YYYY"
int isValidDateFormat(const char* dateStr) {
    // Check length
    if (strlen(dateStr) != 10) {
        return 0;
    }

    // Check for correct hyphen positions
    if (dateStr[2] != '-' || dateStr[5] != '-') {
        return 0;
    }

    // Check if all other characters are digits
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue; // Skip hyphens
        if (!isdigit(dateStr[i])) return 0;
    }

    return 1;
}

// Function to extract day, month, and year from the date string
void parseDate(const char* dateStr, int* day, int* month, int* year) {
    sscanf(dateStr, "%d-%d-%d", day, month, year);
}

// Function to compare two dates
int compareDates(const char* dateStr1, const char* dateStr2) {
    int day1, month1, year1;
    int day2, month2, year2;

    // Validate date formats
    if (!isValidDateFormat(dateStr1) || !isValidDateFormat(dateStr2)) {
        return -2; // Return error code for invalid format
    }

    // Parse the dates
    parseDate(dateStr1, &day1, &month1, &year1);
    parseDate(dateStr2, &day2, &month2, &year2);

    // Compare year, then month, then day
    if (year1 < year2) return -1;
    if (year1 > year2) return 1;

    if (month1 < month2) return -1;
    if (month1 > month2) return 1;

    if (day1 < day2) return -1;
    if (day1 > day2) return 1;

    return 0; // Dates are equal
}