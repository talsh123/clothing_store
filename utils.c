// Tal-Shalom Ben Ovadia 322356346 
// Stav Moalem 211500657
// Rafi Erez 301420352

#define _CRT_SECURE_NO_WARNINGS

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void write_log(const char* filename, const char* message)
{
    FILE* file = fopen(filename, "a");
    if (!file)
    {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);
    struct tm* time_info = localtime(&now);
    char timestamp[80];

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);
    fprintf(file, "[%s] %s\n", timestamp, message);

    fclose(file);
}

void display_log()
{
    FILE* file = fopen("log.txt", "r");
    if (!file)
    {
        perror("Failed to open log file");
        return;
    }

    printf("Transaction Log:\n");

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file))
        printf("%s", buffer);

    fclose(file);
}