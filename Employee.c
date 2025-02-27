#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Employee.h"
#include "Customer.h"
#include "Item.h"
#include "StringOperations.h"
#include <assert.h>

#define EMPLOYEES_FILE "employees.txt"

// Employee FILE properties LIMIT
#define USERNAME_LENGTH 64
#define PRIVATE_NAME_LENGTH 64
#define PASSWORD_LENGTH 128
#define LEVEL_LENGTH 2

// User Validation Attemps
#define ALLOWED_ATTEMPS 3

Employee* createEmployee(char* username, char* privateName, char* password, char* level) {
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
    strncpy(employee->level, level, strlen(level));
    employee->level[strlen(level)] = '\0'; // Ensure null termination

    return employee;
}

void writeEmployee(Employee* Employee, const char* fileName) {
    FILE* file = fopen(fileName, "a");

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

    // Buffer to store the line from the file
    char line[512]; // Adjust size as needed

    // Read a line from the file
    if (fgets(line, sizeof(line), file) == NULL) {
        // If fgets() returns NULL, it means end-of-file or an error occurred
        return NULL;
    }

    // Allocate memory for Employee
    Employee* employee = (Employee*)malloc(sizeof(Employee));
    if (employee == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    // Allocate memory for strings
    employee->username = (char*)malloc(USERNAME_LENGTH);
    employee->privateName = (char*)malloc(PRIVATE_NAME_LENGTH);
    employee->password = (char*)malloc(PASSWORD_LENGTH);
    employee->level = (char*)malloc(LEVEL_LENGTH);

    // Validate memory allocation
    if (!employee->username || !employee->privateName ||
        !employee->password || !employee->level) {
        printf("Error: Memory allocation failed.\n");
        free(employee); // Free employee struct memory
        return NULL;
    }

    // Parse the line using sscanf
    int numParsed = sscanf(line, "%s %s %s %s",
        employee->username,
        employee->privateName,
        employee->password,
        employee->level
    );

    // If all 4 fields are not parsed, free memory and return NULL
    if (numParsed != 4) {
        free(employee->username);
        free(employee->privateName);
        free(employee->password);
        free(employee->level);
        free(employee);
        return NULL;
    }

    return employee;
}

Employee* checkCredentials(char* username, char* password) {
    FILE* file = fopen(EMPLOYEES_FILE, "r");
    if (file == NULL) {
        printf("Error: Could not open file.\n");
        return NULL;
    }

    while (!feof(file)) {  // Check if end of file is reached
        Employee* employee = readEmployee(file);

        // Validate the employee object
        if (employee == NULL) {
            break;  // Exit the loop if no more Employees can be read
        }

        // Compare the credentials
        if (!strcmp(employee->username, username) && !strcmp(employee->password, password)) {
            fclose(file);  // Close the file before returning
            return employee; // Credentials match
        }

        // Free the employee if credentials do not match
        free(employee->username);
        free(employee->privateName);
        free(employee->password);
        free(employee->level);
        free(employee);
    }

    fclose(file); // Close the file before returning
    return NULL; // Credentials do not match
}

// Function to print all employees in one line each
void printEmployees(Employee* employees, int employeeCount) {
    if (employees == NULL || employeeCount <= 0) {
        printf("No employees to display.\n");
        return;
    }

    printf("\n--- Employees List ---\n");
    for (int i = 0; i < employeeCount; i++) {
        printf("%s, %s, %s, %s\n",
            employees[i].username,
            employees[i].privateName,
            employees[i].password,
            employees[i].level
        );
    }
    printf("--------------------\n");
}

Employee* getAllEmployees() {
    FILE* fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return NULL;
    }

    Employee* employees = NULL;
    int employeeCount = 0;
    Employee* tempEmployee;

    // Read all Employees
    while ((tempEmployee = readEmployee(fp)) != NULL) {
        // Dynamically allocate memory
        employees = realloc(employees, sizeof(Item) * (employeeCount + 1));

        // Hard copy the Item
        employees[employeeCount].username = (char*)malloc(sizeof(char) * USERNAME_LENGTH);
        employees[employeeCount].privateName = (char*)malloc(sizeof(char) * PRIVATE_NAME_LENGTH);
        employees[employeeCount].password = (char*)malloc(sizeof(char) * PASSWORD_LENGTH);
        employees[employeeCount].level = (char*)malloc(sizeof(char) * LEVEL_LENGTH);
        strcpy(employees[employeeCount].username, (*tempEmployee).username);
        strcpy(employees[employeeCount].privateName, (*tempEmployee).privateName);
        strcpy(employees[employeeCount].password, (*tempEmployee).password);
        strcpy(employees[employeeCount].level, (*tempEmployee).level);

        // Increment the itemCount
        employeeCount++;

        // Check for end of file after attempting to read
        if (feof(fp)) {
            break;
        }
    }

    fclose(fp);
    printEmployees(employees, employeeCount);
    return employees;
}

int menuItems(int level, int* mapping) {
    int optionNumber = 1;  // Start numbering from 1

    printf("Welcome to the RTS System!\n");
    printf("Menu:\n");

    // Trainee Level (3 and below)
    if (level <= 3) {
        printf("%d. View Items.\n", optionNumber);
        mapping[optionNumber++] = 1;

        printf("%d. Add New Item.\n", optionNumber);
        mapping[optionNumber++] = 2;

        printf("%d. Add New Customer.\n", optionNumber);
        mapping[optionNumber++] = 3;

        printf("%d. View Customer Purchases.\n", optionNumber);
        mapping[optionNumber++] = 4;
    }

    // Worker Level (2 and below)
    if (level <= 2) {
        printf("%d. Remove an Item.\n", optionNumber);
        mapping[optionNumber++] = 5;

        printf("%d. Remove a Customer.\n", optionNumber);
        mapping[optionNumber++] = 6;

        printf("%d. Update Item.\n", optionNumber);
        mapping[optionNumber++] = 7;

        printf("%d. Sell an Item.\n", optionNumber);
        mapping[optionNumber++] = 8;
    }

    // Admin Level (1 only)
    if (level == 1) {
        printf("%d. Add New Employee.\n", optionNumber);
        mapping[optionNumber++] = 9;

        printf("%d. Show all Employees.\n", optionNumber);
        mapping[optionNumber++] = 11;
    }

    // Common Options for All Levels
    printf("%d. Show all Items.\n", optionNumber);
    mapping[optionNumber++] = 10;

    printf("%d. Show all Customers.\n", optionNumber);
    mapping[optionNumber++] = 12;

    printf("%d. Exit the RTS System.\n", optionNumber);
    mapping[optionNumber++] = 13;

    return optionNumber - 1; // Return total number of options
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
    printf("No employees.txt file found!\n");
    printf("employees.txt file created and signed you in as Admin!\n");
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
                // Deep copy each string
                strncpy(tempEmployee.username, employeePtr->username, USERNAME_LENGTH);
                strncpy(tempEmployee.privateName, employeePtr->privateName, PRIVATE_NAME_LENGTH);
                strncpy(tempEmployee.password, employeePtr->password, PASSWORD_LENGTH);
                strncpy(tempEmployee.level, employeePtr->level, LEVEL_LENGTH);

                free(username);
                free(password);

                return tempEmployee;
            }
            else {
                if (user_attempts == ALLOWED_ATTEMPS - 1) {
                    printf("You have entered incorrect credentials 3 times. Please try again later.\n");
                    exit(EXIT_FAILURE);
                }
                else {
                    printf("Incorrect credentials. Please try again!\n");
                }
                user_attempts++;
            }
        }
    }
    // Default return for compiler, shouldn't reach here
    return tempEmployee;
}

void addNewEmployee() {
    clrscr();
    char* username = (char*)malloc(sizeof(char) * USERNAME_LENGTH);
    char* privateName = (char*)malloc(sizeof(char) * PRIVATE_NAME_LENGTH);
    char* password = (char*)malloc(sizeof(char) * PASSWORD_LENGTH);
    char* level = (char*)malloc(sizeof(char) * LEVEL_LENGTH);
    clearBuffer();
    printf("Add Item Menu:\n");
    printf("Please enter Username: ");
    scanf("%s", username);
    printf("Please enter Private Name: ");
    scanf("%s", privateName);
    printf("Please enter Password: ");
    scanf("%s", password);
    printf("Please enter level: ");
    scanf("%s", level);
    Employee* employee = createEmployee(username, privateName, password, level);
    writeEmployee(employee, EMPLOYEES_FILE);
    printItems(employee, 1);
    printf("Employee has been added successfully!\n");
}

void showMenu() {
    while (1) {
        int exit = 0;
        int mapping[20] = { 0 };  // Array to map options to switch cases

        clrscr();
        int totalOptions = menuItems(atoi(currentEmployee.level), mapping);

        int user_choice;
        printf("Please enter a choice: ");
        scanf("%d", &user_choice);

        // Validate the user choice
        if (user_choice < 1 || user_choice > totalOptions || mapping[user_choice] == 0) {
            printf("Invalid choice. Please try again.\n");
            continue;
        }

        switch (mapping[user_choice]) {
        case 1:
            clrscr();
            viewItems();
            break;
        case 2:
            clrscr();
            addNewItem();
            break;
        case 3: {
            char user;
            clrscr();
            addNewCustomer();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 4: {
            char user;
            clrscr();
            checkCustomerPurchases();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 5: {
            char user;
            clrscr();
            removeItemMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 6: {
            char user;
            clrscr();
            removeCustomerMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 7: {
            char user;
            clrscr();
            updateItemMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 8: {
            char user;
            clrscr();
            sellItemMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 9: {
            char user;
            clrscr();
            addNewEmployee();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 10: {
            char user;
            int itemCount = 0;
            Item* items = getAllItems(&itemCount);
            printItems(items, itemCount);
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 11: {
            char user;
            getAllEmployees();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 12: {
            char user;
            int customersCount;
            Customer* customers = getAllCustomers(&customersCount);
            printCustomers(customers, customersCount);
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 13:
            exit = 1;
            break;
        }
        if (exit == 1)
            break;
    }
}