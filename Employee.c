#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Employee.h"
#include "FileOperations.h"
#include "StringOperations.h"
#include <assert.h>

#define EMPLOYEES_FILE "Employees.txt"

// Employee FILE properties LIMIT
#define USERNAME_LENGTH 64
#define PRIVATE_NAME_LENGTH 64
#define PASSWORD_LENGTH 128
#define LEVEL_LENGTH 2

// User Validation Attemps
#define ALLOWED_ATTEMPS 3


Employee* createEmployee(char* username, char* privateName, char* password, char* permissionLevel) {
    Employee* employee = (Employee*)malloc(sizeof(Employee));

    employee->username = (char*)malloc(USERNAME_LENGTH);
    strncpy(employee->username, username, strlen(username));
    employee->username[strlen(username)] = '\0'; // Ensure null termination

    employee->privateName = (char*)malloc(PRIVATE_NAME_LENGTH);
    strncpy(employee->privateName, privateName, strlen(privateName));
    employee->privateName[strlen(privateName)] = '\0'; // Ensure null termination

    employee->password = (char*)malloc(PASSWORD_LENGTH);
    strncpy(employee->password, password, strlen(password));
    employee->password[strlen(password)] = '\0'; // Ensure null termination

    employee->level = (char*)malloc(LEVEL_LENGTH);
    strncpy(employee->level, permissionLevel, strlen(permissionLevel));
    employee->level[strlen(permissionLevel)] = '\0'; // Ensure null termination

    return employee;
}

void writeEmployee(Employee* Employee, const char* fileName) {
    FILE* file = fopen(fileName, "w");

    // %-*s - string is left-aligned and padded with spaces up to the specified length

    // Write username with fixed length
    fprintf(file, "%-*s", USERNAME_LENGTH, Employee->username);

    // Write privateName with fixed length
    fprintf(file, "%-*s", PRIVATE_NAME_LENGTH, Employee->privateName);

    // Write password with fixed length
    fprintf(file, "%-*s", PASSWORD_LENGTH, Employee->password);

    // Write permissionLevel with fixed length (convert integer to string if necessary)
    fprintf(file, "%-*s", LEVEL_LENGTH, Employee->level);

    fclose(file);
}

Employee* readEmployee(FILE* file) {
    if (file == NULL) {
        return NULL; // File pointer is NULL
    }

    // Dynamically allocates the Employee properties
    char* temp_username = (char*)malloc(USERNAME_LENGTH * sizeof(char));
    char* temp_private_name = (char*)malloc(PRIVATE_NAME_LENGTH * sizeof(char));
    char* temp_password = (char*)malloc(PASSWORD_LENGTH * sizeof(char));
    char* temp_permission_level = (char*)malloc(LEVEL_LENGTH * sizeof(char));

    // Validate memory allocation
    assert(temp_username);
    assert(temp_private_name);
    assert(temp_password);
    assert(temp_permission_level);

    // Read details of the Employee
    fread(temp_username, sizeof(char), USERNAME_LENGTH, file);
    fread(temp_private_name, sizeof(char), PRIVATE_NAME_LENGTH, file);
    fread(temp_password, sizeof(char), 128, file);
    fread(temp_permission_level, sizeof(char), LEVEL_LENGTH, file);

    temp_username = trimwhitespace(temp_username);
    temp_private_name = trimwhitespace(temp_private_name);
    temp_password = trimwhitespace(temp_password);
    temp_permission_level = trimwhitespace(temp_permission_level);

    // Creates a new Employee
    Employee* Employee = createEmployee(temp_username, temp_private_name, temp_password, temp_permission_level);

    return Employee;
}

Employee* checkCredentials(char* username, char* password) {
    FILE* file = fopen(EMPLOYEES_FILE, "r");
    while (1) {
        Employee* employee = readEmployee(file);
        if (employee == NULL) {
            break; // Exit the loop if no more Employees can be read
        }

        // Compare the credentials
        if (!strcmp(employee->username, username) && !strcmp(employee->password, password)) {
            return employee; // Credentials match
        }
    }
    return NULL; // Credentials do not match
}

void menuItems(int level) {
    if (level <= '3') {
        printf("1. View Items.\n");
        printf("2. Add New Item.\n");
        printf("3. Add New Customer.\n");
        printf("4. View Customer Purchases.\n");
    }
    if (level <= '2') {
        printf("5. Remove an Item.\n");
        printf("6. Remove a Customer.\n");
        printf("7. Update Item.\n");
        printf("8. Sell an Item.\n");
    }
    if (level == '1') {
        printf("9. Add New Employee.\n");
    }
}

int checkIfEmployeeFileExists() {
    FILE* file;
    if ((file = fopen(EMPLOYEES_FILE, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}

void createDefaultAdmin() {
    Employee* employee = createEmployee("admin", "manager", "12345678", "1");
    writeEmployee(employee, EMPLOYEES_FILE);
    printf("Employees.txt file was created!\n");
}

Employee login() {
    Employee tempEmployee;
    tempEmployee.username = (char*)malloc(USERNAME_LENGTH * sizeof(char));
    tempEmployee.privateName = (char*)malloc(PRIVATE_NAME_LENGTH * sizeof(char));
    tempEmployee.password = (char*)malloc(PASSWORD_LENGTH * sizeof(char));
    tempEmployee.level = (char*)malloc(LEVEL_LENGTH * sizeof(char));

    if (!checkIfEmployeeFileExists()) {
        // Create default admin
        strcpy(tempEmployee.username, "admin");
        strcpy(tempEmployee.privateName, "manager");
        strcpy(tempEmployee.password, "12345678");
        strcpy(tempEmployee.level, "1");
        return tempEmployee;
    }
    else {
        int user_attempts = 0;
        char* username = (char*)malloc(USERNAME_LENGTH * sizeof(char));
        char* password = (char*)malloc(PASSWORD_LENGTH * sizeof(char));

        assert(username != NULL);
        assert(password != NULL);

        while (user_attempts < ALLOWED_ATTEMPS) {
            printf("Please enter username: ");
            scanf("%s", username);
            printf("Please enter password: ");
            scanf("%s", password);

            Employee* employeePtr = checkCredentials(username, password);

            if (employeePtr != NULL) {
                printf("Welcome to the RTS System!\n");

                // Deep copy each string
                strncpy(tempEmployee.username, employeePtr->username, USERNAME_LENGTH);
                strncpy(tempEmployee.privateName, employeePtr->privateName, PRIVATE_NAME_LENGTH);
                strncpy(tempEmployee.password, employeePtr->password, PASSWORD_LENGTH);
                strncpy(tempEmployee.level, employeePtr->level, LEVEL_LENGTH);

                // Ensure null termination
                tempEmployee.username[USERNAME_LENGTH - 1] = '\0';
                tempEmployee.privateName[PRIVATE_NAME_LENGTH - 1] = '\0';
                tempEmployee.password[PASSWORD_LENGTH - 1] = '\0';
                tempEmployee.level[LEVEL_LENGTH - 1] = '\0';

                free(username);
                free(password);

                return tempEmployee;
            }
            else {
                printf("Access Denied!\n");
                if (user_attempts == ALLOWED_ATTEMPS - 1) {
                    printf("You have entered incorrect credentials three times. Please try again later.\n");
                    exit(EXIT_FAILURE);
                }
                user_attempts++;
            }
        }
    }
    // Default return for compiler, shouldn't reach here
    return tempEmployee;
}

void showMenu() {
    FILE* file = fopen(EMPLOYEES_FILE, "r");
    Employee* employee = readEmployee(file);
    printf("Menu:\n");
    char level = *(employee->level);
    menuItems(level);
    while (1) {
        int user_choice;
        clearBuffer();
        printf("please enter a choice: ");
        scanf("%d", &user_choice);
        switch (user_choice) {
        }
    }
}