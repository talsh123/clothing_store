#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "StringOperations.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char* trimwhitespace(char* str) {
    // Skip leading whitespace
    while (isspace((unsigned char)*str)) str++;

    // If all characters were spaces
    if (*str == 0)
        return strdup(""); // Return an empty string

    // Find the end of the string
    char* end = str + strlen(str) - 1;

    // Move backward to remove trailing whitespace
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Set the new end of the string
    end[1] = '\0';

    // Return the trimmed string
    return strdup(str);
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

// Function to check if dateToCheck is between startDate and endDate
int isDateBetween(const char* startDate, const char* endDate, const char* dateToCheck) {
    // Validate all three dates
    if (!isValidDateFormat(startDate) || !isValidDateFormat(endDate) || !isValidDateFormat(dateToCheck)) {
        return 0;
    }

    // Compare the dates
    int cmpStart = compareDates(startDate, dateToCheck); // startDate <= dateToCheck
    int cmpEnd = compareDates(dateToCheck, endDate);     // dateToCheck <= endDate

    // Check if dateToCheck is between startDate and endDate (inclusive)
    if (cmpStart <= 0 && cmpEnd <= 0) {
        return 1;
    }
    else {
        return 0;
    }
}

// Function to trim a string to a maximum length and return it
char* trimStringToLength(char* str, int maxLength) {
    // Check for NULL pointer or non-positive length
    if (str == NULL || maxLength <= 0) {
        return "";
    }

    // Trim whitespace from start and end
    str = trimwhitespace(str);

    // Get the current length of the string
    int currentLength = strlen(str);

    // If the string is longer than maxLength, truncate it
    if (currentLength > maxLength) {
        str[maxLength] = '\0';  // Null-terminate at maxLength
    }

    return str;
}

// Function to safely get a string with a maximum length
void getInputString(char* str, int maxLength) {
    // Check for NULL pointer or invalid length
    if (str == NULL || maxLength <= 1) {
        return;
    }

    // Get the input with fgets()
    fgets(str, maxLength, stdin);

    // Remove the newline character if present
    str[strcspn(str, "\n")] = 0;

    // If the input was too long and there are leftover characters
    if (strlen(str) == maxLength - 1 && str[maxLength - 2] != '\n') {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);  // Clear the input buffer
    }

    // Ensure null-termination
    str[maxLength - 1] = '\0';
}