#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "FileOperations.h"

int isFileExists(char* fname) {
    FILE* file;
    if ((file = fopen(fname, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}