// Tal-Shalom Ben Ovadia 322356346 
// Stav Moalem 211500657
// Rafi Erez 301420352

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "StringOperations.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Item.h"

char* trimwhitespace(char* str) {
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)
        return strdup("");

    char* end = str + strlen(str) - 1;

    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

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

int isValidDateFormat(const char* dateStr) {
    if (strlen(dateStr) != 10) {
        return 0;
    }

    if (dateStr[2] != '-' || dateStr[5] != '-') {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue; // Skip hyphens
        if (!isdigit(dateStr[i])) return 0;
    }

    return 1;
}

void parseDate(const char* dateStr, int* day, int* month, int* year) {
    sscanf(dateStr, "%d-%d-%d", day, month, year);
}

int compareDates(const char* dateStr1, const char* dateStr2) {
    int day1, month1, year1;
    int day2, month2, year2;

    if (!isValidDateFormat(dateStr1) || !isValidDateFormat(dateStr2)) {
        return -2;
    }

    parseDate(dateStr1, &day1, &month1, &year1);
    parseDate(dateStr2, &day2, &month2, &year2);

    if (year1 < year2) return -1;
    if (year1 > year2) return 1;

    if (month1 < month2) return -1;
    if (month1 > month2) return 1;

    if (day1 < day2) return -1;
    if (day1 > day2) return 1;

    return 0; // Dates are equal
}

int isDateBetween(const char* startDate, const char* endDate, const char* dateToCheck) {
    if (!isValidDateFormat(startDate) || !isValidDateFormat(endDate) || !isValidDateFormat(dateToCheck)) {
        return 0;
    }

    int cmpStart = compareDates(startDate, dateToCheck);
    int cmpEnd = compareDates(dateToCheck, endDate);

    if (cmpStart <= 0 && cmpEnd <= 0) {
        return 1;
    }
    else {
        return 0;
    }
}

void getInputString(char* str, int maxLength) {
    if (str == NULL || maxLength <= 1) {
        return;
    }

    fgets(str, maxLength, stdin);

    str[strcspn(str, "\n")] = 0;

    if (strlen(str) == maxLength - 1 && str[maxLength - 2] != '\n') {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);  // Clear the input buffer
    }

    str[maxLength - 1] = '\0';
}

char* getCurrentDate() {
    char* dateStr = (char*)malloc(11); // DD-MM-YYYY + null terminator
    if (dateStr == NULL) {
        printf("Error: Memory allocation failed for date string.\n");
        return NULL;
    }

    time_t t = time(NULL);
    if (t == -1) {
        printf("Error: Failed to get current time.\n");
        free(dateStr);
        return NULL;
    }

    struct tm* tm = localtime(&t);
    if (tm == NULL) {
        printf("Error: Failed to convert to local time.\n");
        free(dateStr);
        return NULL;
    }

    snprintf(dateStr, 11, "%02d-%02d-%04d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);

    return dateStr;
}

int checkIf14DaysHavePassed(const char* date1, const char* date2) {
    struct tm tm1 = { 0 };
    struct tm tm2 = { 0 };

    sscanf(date1, "%2d-%2d-%4d", &tm1.tm_mday, &tm1.tm_mon, &tm1.tm_year);
    sscanf(date2, "%2d-%2d-%4d", &tm2.tm_mday, &tm2.tm_mon, &tm2.tm_year);

    tm1.tm_mon -= 1;
    tm1.tm_year -= 1900;
    tm2.tm_mon -= 1;
    tm2.tm_year -= 1900;

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    if (time1 == -1 || time2 == -1) {
        printf("Error: Invalid date format.\n");
        return 0;
    }

    double difference = difftime(time1, time2);

    double daysDifference = difference / (60 * 60 * 24);

    return (abs(daysDifference) < 14) ? 1 : 0;
}

int compareLevels(char* level1, char* level2) {
    int num1 = atoi(level1);
    int num2 = atoi(level2);

    if (num1 > num2) return 1;
    if (num1 < num2) return -1;
    return 0;
}

void* safeMalloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Memory allocation of %zu bytes failed.\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void Logger(const char* act, const char* att, const char* value, const char* msg)
{
    time_t t = time(NULL);
    printf("\n%s\n", msg);

    FILE* aa = fopen("l.log", "a");
    if (aa == NULL) {
        perror("Error opening log file");
        return;
    }

    fprintf(aa, "Action: %s Attribute: %s Value: %s Time: %s", act, att, value, ctime(&t));
    fflush(aa);
    fclose(aa);
}

int getLinkedListSize(ItemNode* head) {
    int size = 0;
    ItemNode* current = head;

    // Iterate over the linked list
    while (current != NULL) {
        size++;
        current = current->next;
    }

    return size;
}