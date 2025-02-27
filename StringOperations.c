#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "StringOperations.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

// Function to get the current date as a string in the format DD-MM-YYYY
char* getCurrentDate() {
    // Allocate memory for the date string
    char* dateStr = (char*)malloc(11); // DD-MM-YYYY + null terminator
    if (dateStr == NULL) {
        printf("Error: Memory allocation failed for date string.\n");
        return NULL;
    }

    // Get the current time
    time_t t = time(NULL);
    if (t == -1) {
        printf("Error: Failed to get current time.\n");
        free(dateStr);
        return NULL;
    }

    // Convert to local time
    struct tm* tm = localtime(&t);
    if (tm == NULL) {
        printf("Error: Failed to convert to local time.\n");
        free(dateStr);
        return NULL;
    }

    // Format the date as DD-MM-YYYY
    snprintf(dateStr, 11, "%02d-%02d-%04d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);

    return dateStr;
}

// Function to compare two dates in DD-MM-YYYY format
int checkIf14DaysHavePassed(const char* date1, const char* date2) {
    struct tm tm1 = { 0 };
    struct tm tm2 = { 0 };

    // Parse the date strings
    sscanf(date1, "%2d-%2d-%4d", &tm1.tm_mday, &tm1.tm_mon, &tm1.tm_year);
    sscanf(date2, "%2d-%2d-%4d", &tm2.tm_mday, &tm2.tm_mon, &tm2.tm_year);

    // Adjust month and year for mktime
    tm1.tm_mon -= 1;
    tm1.tm_year -= 1900;
    tm2.tm_mon -= 1;
    tm2.tm_year -= 1900;

    // Convert to time_t
    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    if (time1 == -1 || time2 == -1) {
        printf("Error: Invalid date format.\n");
        return 0;
    }

    // Calculate the difference in seconds
    double difference = difftime(time1, time2);

    // Convert to days
    double daysDifference = difference / (60 * 60 * 24);

    // Check if the difference is less than 14 days
    return (abs(daysDifference) < 14) ? 1 : 0;
}