#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Worker.h"
#include "FileOperations.h"
#include <assert.h>

#define WORKERS_FILE "workers.txt"

// Worker FILE STRUCTURE
#define USERNAME_LENGTH 64
#define PRIVATE_NAME_LENGTH 64
#define PASSWORD_LENGTH 128
#define PERMISSION_LEVEL_LENGTH 1

char* trimwhitespace(char* str) {

    // Skip leading whitespace
    while (isspace((unsigned char)*str))
        str++;

    // Set the start of the string
    char* start = str;

    // Forward till the end of the string
    while (!isspace((unsigned char)*str))
        str++;
    char* end = str--;

    // Calculate the length of the trimmed string and allocate memory
    size_t newLength = end - start;

    char* newString = (char*)malloc(newLength); // +1 for null terminator
    strncpy(newString, start, newLength);
    newString[newLength] = '\0';
    return newString;
}


Worker* createWorker(char* username, char* privateName, char* password, char* permissionLevel) {
    Worker* worker = (Worker*)malloc(sizeof(Worker));

    worker->username = (char*)malloc(USERNAME_LENGTH);
    strcpy(worker->username, username);

    worker->privateName = (char*)malloc(PRIVATE_NAME_LENGTH);
    strcpy(worker->privateName, privateName);

    worker->password = (char*)malloc(PASSWORD_LENGTH);
    strcpy(worker->password, password);

    worker->permissionLevel = (char*)malloc(PERMISSION_LEVEL_LENGTH);
    strcpy(worker->permissionLevel, permissionLevel);

    return worker;
}

int compareWorkers(const Worker* worker1, const Worker* worker2) {
    // Check if usernames are the same
    if (strcmp(worker1->username, worker2->username) != 0) {
        return 0;
    }

    // Check if private names are the same
    if (strcmp(worker1->privateName, worker2->privateName) != 0) {
        return 0;
    }

    // Check if passwords are the same
    if (strcmp(worker1->password, worker2->password) != 0) {
        return 0;
    }

    // Check if permission levels are the same
    if (strcmp(worker1->permissionLevel, worker2->permissionLevel) != 0) {
        return 0;
    }

    // All fields are the same
    return 1;
}

void writeWorker(Worker* worker, const char* fileName) {
    FILE* file = fopen(fileName, "w");

    // %-*s - string is left-aligned and padded with spaces up to the specified length

    // Write username with fixed length
    fprintf(file, "%-*s", USERNAME_LENGTH, worker->username);

    // Write privateName with fixed length
    fprintf(file, "%-*s", PRIVATE_NAME_LENGTH, worker->privateName);

    // Write password with fixed length
    fprintf(file, "%-*s", PASSWORD_LENGTH, worker->password);

    // Write permissionLevel with fixed length (convert integer to string if necessary)
    fprintf(file, "%-*s", PERMISSION_LEVEL_LENGTH, worker->permissionLevel);

    fclose(file);
}

Worker* readWorker(FILE* file) {
    if (file == NULL) {
        return NULL; // File pointer is NULL
    }

    // Dynamically allocates the Worker properties
    char* temp_username = (char*)malloc(USERNAME_LENGTH * sizeof(char));
    char* temp_private_name = (char*)malloc(PRIVATE_NAME_LENGTH * sizeof(char));
    char* temp_password = (char*)malloc(PASSWORD_LENGTH * sizeof(char));
    char* temp_permission_level = (char*)malloc(PERMISSION_LEVEL_LENGTH * sizeof(char));

    if (temp_username == NULL || temp_private_name == NULL || temp_password == NULL || temp_permission_level == NULL) {
        // Memory allocation failed, clean up and return NULL
        free(temp_username);
        free(temp_private_name);
        free(temp_password);
        free(temp_permission_level);
        return NULL;
    }

    // Read details of the worker
    size_t read_username = fread(temp_username, sizeof(char), USERNAME_LENGTH, file);
    size_t read_private_name = fread(temp_private_name, sizeof(char), PRIVATE_NAME_LENGTH, file);
    size_t read_password = fread(temp_password, sizeof(char), PASSWORD_LENGTH, file);
    size_t read_permission_level = fread(temp_permission_level, sizeof(char), PERMISSION_LEVEL_LENGTH, file);

    // Check if all fields were read successfully
    if (read_username != USERNAME_LENGTH || read_private_name != PRIVATE_NAME_LENGTH || read_password != PASSWORD_LENGTH || read_permission_level != PERMISSION_LEVEL_LENGTH) {
        // Free memory and return NULL if EOF or error occurred
        free(temp_username);
        free(temp_private_name);
        free(temp_password);
        free(temp_permission_level);
        return NULL;
    }

    temp_username = trimwhitespace(temp_username);
    temp_private_name = trimwhitespace(temp_private_name);
    temp_password = trimwhitespace(temp_password);
    temp_permission_level = trimwhitespace(temp_permission_level);

    // Creates a new worker
    Worker* worker = createWorker(temp_username, temp_private_name, temp_password, temp_permission_level);

    return worker;
}

int compareStrings(const char* str1, const char* str2) {
    // Check if both strings are non-NULL
    if (str1 == NULL || str2 == NULL) {
        return 0; // NULL strings cannot be identical
    }

    // Use strcmp to compare the strings
    if (strcmp(str1, str2) == 0) {
        return 1; // Strings are identical
    }
    else {
        return 0; // Strings are different
    }
}

int checkCredentials(char* username, char* password) {
    FILE* file = fopen(WORKERS_FILE, "r");
    while (1) {
        Worker* worker = readWorker(file);
        if (worker == NULL) {
            break; // Exit the loop if no more workers can be read
        }

        // Compare the credentials
        if (compareStrings(worker->username, username) && compareStrings(worker->password, password)) {
            return 1; // Credentials match
        }
    }
    return 0; // Credentials do not match
}

void identifyWorker() {
    // Check if the file exists, otherwise create it
    if (!isFileExists(WORKERS_FILE)) {
        // Create a new admin worker and write to file
        Worker* worker = createWorker("admin", "manager", "12345678", "1");
        writeWorker(worker, WORKERS_FILE);
        printf("No workers file exists! File created and you signed in as an Admin!\n");
    }
    else {
        // Check for existing worker
        char* username = (char*)malloc(USERNAME_LENGTH * sizeof(char));
        char* password = (char*)malloc(PASSWORD_LENGTH * sizeof(char));

        assert(username != NULL);
        assert(password != NULL);

        printf("Please enter username: ");
        scanf("%63s", username);

        printf("Please enter password: ");
        scanf("%127s", password);

        if (checkCredentials(username, password)) {
            printf("Granted Access!\n");
        }
        else {
            printf("Access Denied!\n");
        }
    }
}