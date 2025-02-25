#ifndef STRINGOPERATIONS_H
#define STRINGOPERATIONS_H

char* trimwhitespace(char* str);
int compareStrings(const char* str1, const char* str2);
void clearBuffer();
void clrscr();
void parseDate(const char* dateStr, int* day, int* month, int* year);
int compareDates(const char* dateStr1, const char* dateStr2);
int isValidDateFormat(const char* dateStr);
int isDateBetween(const char* startDate, const char* endDate, const char* dateToCheck);

#endif // STRINGOPERATIONS_H