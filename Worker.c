#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Worker.h"
#include "FileOperations.h"
#include "StringOperations.h"
#include <assert.h>

#define WORKERS_FILE "workers.txt"

// Worker FILE properties LIMIT
#define USERNAME_LENGTH 64
#define PRIVATE_NAME_LENGTH 64
#define PASSWORD_LENGTH 128
#define PERMISSION_LEVEL_LENGTH 1

// User Validation Attemps
#define ALLOWED_ATTEMPS 3


Worker* createWorker(char* username, char* privateName, char* password, char* permissionLevel) {
    Worker* worker = (Worker*)malloc(sizeof(Worker));

    worker->username = (char*)malloc(USERNAME_LENGTH);
    strncpy(worker->username, username, strlen(username));
    worker->username[strlen(username)] = '\0'; // Ensure null termination

    worker->privateName = (char*)malloc(PRIVATE_NAME_LENGTH);
    strncpy(worker->privateName, privateName, strlen(privateName));
    worker->privateName[strlen(privateName)] = '\0'; // Ensure null termination

    worker->password = (char*)malloc(PASSWORD_LENGTH);
    strncpy(worker->password, password, strlen(password));
    worker->password[strlen(password)] = '\0'; // Ensure null termination

    worker->permissionLevel = (char*)malloc(PERMISSION_LEVEL_LENGTH);
    strncpy(worker->permissionLevel, permissionLevel, strlen(permissionLevel));
    worker->permissionLevel[strlen(permissionLevel)] = '\0'; // Ensure null termination

    return worker;
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

    // Validate memory allocation
    assert(temp_username);
    assert(temp_private_name);
    assert(temp_password);
    assert(temp_permission_level);

    // Read details of the worker
    fread(temp_username, sizeof(char), USERNAME_LENGTH, file);
    fread(temp_private_name, sizeof(char), PRIVATE_NAME_LENGTH, file);
    fread(temp_password, sizeof(char), PASSWORD_LENGTH, file);
    fread(temp_permission_level, sizeof(char), PERMISSION_LEVEL_LENGTH, file);

    temp_username = trimwhitespace(temp_username);
    temp_private_name = trimwhitespace(temp_private_name);
    temp_password = trimwhitespace(temp_password);
    temp_permission_level = trimwhitespace(temp_permission_level);

    // Creates a new worker
    Worker* worker = createWorker(temp_username, temp_private_name, temp_password, temp_permission_level);

    return worker;
}

int checkCredentials(char* username, char* password) {
    FILE* file = fopen(WORKERS_FILE, "r");
    while (1) {
        Worker* worker = readWorker(file);
        if (worker == NULL) {
            break; // Exit the loop if no more workers can be read
        }

        // Compare the credentials
        if (!strcmp(worker->username, username) && !strcmp(worker->password, password)) {
            return 1; // Credentials match
        }
    }
    return 0; // Credentials do not match
}

void menuItems(int permissionLevel) {
    if (permissionLevel <= '3') {
        printf("1. View Items.\n");
        printf("2. Add New Item.\n");
        printf("3. Add New Customer.\n");
        printf("4. View Customer Purchases.\n");
    }
    if (permissionLevel <= '2') {
        printf("5. Remove an Item.\n");
        printf("6. Remove a Customer.\n");
        printf("7. Update Item.\n");
        printf("8. Sell an Item.\n");
    }
    if (permissionLevel == '1') {
        printf("9. Add New Worker.\n");
    }
}

void displayMenu(char* username, char* password) {
    FILE* file = fopen(WORKERS_FILE, "r");
    Worker* worker = readWorker(file);
    printf("Menu:\n");
    char permissionLevel = *(worker->permissionLevel);
    menuItems(permissionLevel);
    while (1) {
        int user_choice;
        clearBuffer();
        printf("please enter a choice: ");
        scanf("%d", &user_choice);
        switch (user_choice) {
        case 1:
            viewItems();    
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        default:
            clrscr();
            printf("No choice was detected, please try again!\n");
            menuItems(permissionLevel);
        }
    }
}

void identifyWorker() {
    int granted_access = 0;
    char* username = NULL;
    char* password = NULL;
    // Check if the file exists, otherwise create it
    if (!isFileExists(WORKERS_FILE)) {
        // Create a new admin worker and write to file
        Worker* worker = createWorker("admin", "manager", "12345678", "1");
        writeWorker(worker, WORKERS_FILE);
        printf("No workers file exists!");
        printf("Workers.txt file was created and signed you in as an Admin!\n");
        granted_access = 1;
    }
    else {
        int user_attemps = 0;
        while (user_attemps < ALLOWED_ATTEMPS) {
            // Check for existing worker
            username = (char*)malloc(USERNAME_LENGTH * sizeof(char));
            password = (char*)malloc(PASSWORD_LENGTH * sizeof(char));

            assert(username != NULL);
            assert(password != NULL);

            printf("Please enter username: ");
            scanf("%s", username);

            printf("Please enter password: ");
            scanf("%s", password);

            if (checkCredentials(username, password)) {
                printf("Granted Access!\n");
                granted_access = 1;
                break;
            }
            else {
                printf("Access Denied!\n");
                if (user_attemps == ALLOWED_ATTEMPS - 1) {
                    printf("You have entered incorrect credentials three times. Please try again later.");
                }

                user_attemps++;
            }
        }
        if (granted_access == 0) {
            printf("Access denied.");
            exit(EXIT_FAILURE);
        }
        else {
            displayMenu(username, password);
        }
    }
}