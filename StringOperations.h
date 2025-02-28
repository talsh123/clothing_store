#ifndef STRINGOPERATIONS_H
#define STRINGOPERATIONS_H

char* trimwhitespace(char* str);
void clearBuffer();
void clrscr();
int isValidDateFormat(const char* dateStr);
void parseDate(const char* dateStr, int* day, int* month, int* year);
int compareDates(const char* dateStr1, const char* dateStr2);
int isDateBetween(const char* startDate, const char* endDate, const char* dateToCheck);
void getInputString(char* str, int maxLength);
char* getCurrentDate();
int checkIf14DaysHavePassed(const char* date1, const char* date2);
int compareLevels(char* level1, char* level2);
void* safeMalloc(size_t size);

#endif // STRINGOPERATIONS_H