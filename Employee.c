#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Employee.h"
#include "Customer.h"
#include "Item.h"
#include "main.h"
#include "StringOperations.h"
#include <assert.h>

Employee* createEmployee(char* username, char* privateName, char* password, char* level) {
    Employee* employee = (Employee*)malloc(sizeof(Employee));
    assert(employee);

    // Allocate and copy username
    employee->username = (char*)malloc(USERNAME_LENGTH);
    assert(employee->username);
    strncpy(employee->username, username, USERNAME_LENGTH - 1);
    employee->username[USERNAME_LENGTH - 1] = '\0';
    employee->username = trimwhitespace(employee->username);

    // Allocate and copy privateName
    employee->privateName = (char*)malloc(PRIVATE_NAME_LENGTH);
    assert(employee->privateName);
    strncpy(employee->privateName, privateName, PRIVATE_NAME_LENGTH - 1);
    employee->privateName[PRIVATE_NAME_LENGTH - 1] = '\0';
    employee->privateName = trimwhitespace(employee->privateName);

    // Allocate and copy password
    employee->password = (char*)malloc(PASSWORD_LENGTH);
    assert(employee->password);
    strncpy(employee->password, password, PASSWORD_LENGTH - 1);
    employee->password[PASSWORD_LENGTH - 1] = '\0';
    employee->password = trimwhitespace(employee->password);

    // Allocate and copy level
    employee->level = (char*)malloc(LEVEL_LENGTH);
    assert(employee->level);
    strncpy(employee->level, level, LEVEL_LENGTH - 1);
    employee->level[LEVEL_LENGTH - 1] = '\0';
    employee->level = trimwhitespace(employee->level);

    // Initialize the next pointer to NULL
    employee->next = NULL;

    printf("Employee '%s' added successfully.\n", username);

    return employee;
}

Employee* addEmployee(Employee* employee) {
    // Initialize the next pointer of the new employee to NULL
    employee->next = NULL;

    // If the list is empty, the new employee becomes the head
    if (globalEmployees == NULL) {
        globalEmployees = employee;
    }
    else {
        // Otherwise, traverse to the end of the list
        Employee* current = globalEmployees;
        while (current->next != NULL) {
            current = current->next;
        }
        // Add the new employee at the end of the list
        current->next = employee;
    }

    printf("Employee '%s' added successfully.\n", employee->username);

    return employee;
}

void writeEmployee(Employee* employee, const char* fileName) {
    FILE* file = fopen(fileName, "a");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", fileName);
        return;
    }

    // Write each field with fixed length, left-aligned and padded with spaces
    fprintf(file, "%-*s", USERNAME_LENGTH - 1, employee->username);
    fprintf(file, "%-*s", PRIVATE_NAME_LENGTH - 1, employee->privateName);
    fprintf(file, "%-*s", PASSWORD_LENGTH - 1, employee->password);
    fprintf(file, "%-*s\n", LEVEL_LENGTH - 1, employee->level);

    fclose(file);
}

void writeEmployees(const char* fileName) {
    // Open the file in write mode to clear its contents
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", fileName);
        return;
    }
    fclose(file); // Close immediately to clear contents

    // Start at the head of the linked list
    Employee* current = globalEmployees;

    // Iterate over the linked list and write each employee to the file
    while (current != NULL) {
        writeEmployee(current, fileName);
        current = current->next;
    }

    printf("All employees have been written to %s successfully.\n", fileName);
}

Employee* readEmployee(FILE* file) {
    if (file == NULL) {
        printf("Error: Invalid file pointer.\n");
        return NULL;
    }

    // Allocate memory for Employee
    Employee* employee = (Employee*)malloc(sizeof(Employee));
    if (employee == NULL) {
        printf("Error: Memory allocation failed for Employee.\n");
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
        printf("Error: Memory allocation failed for strings.\n");
        free(employee->username);
        free(employee->privateName);
        free(employee->password);
        free(employee->level);
        free(employee);
        return NULL;
    }

    // Read each field as fixed-length strings
    if (fgets(employee->username, USERNAME_LENGTH, file) == NULL ||
        fgets(employee->privateName, PRIVATE_NAME_LENGTH, file) == NULL ||
        fgets(employee->password, PASSWORD_LENGTH, file) == NULL ||
        fgets(employee->level, LEVEL_LENGTH, file) == NULL) {

        // Free allocated memory on read failure
        free(employee->username);
        free(employee->privateName);
        free(employee->password);
        free(employee->level);
        free(employee);
        return NULL;
    }

    employee->next = NULL;

    // Free remaining "\n" character in file
    int ch;
    while ((ch = fgetc(file)) != '\n' && ch != EOF);

    // Trim whitespace from each field
    employee->username = trimwhitespace(employee->username);
    employee->privateName = trimwhitespace(employee->privateName);
    employee->password = trimwhitespace(employee->password);
    employee->level = trimwhitespace(employee->level);

    return employee;
}

Employee* checkCredentials(char* username, char* password) {
    // Start at the head of the linked list
    Employee* current = globalEmployees;

    // Traverse the linked list
    while (current != NULL) {
        // Compare the credentials
        if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {
            return current; // Credentials match, return the pointer to this node
        }
        // Move to the next node
        current = current->next;
    }

    printf("Error: Invalid username or password.\n");
    return NULL; // Credentials do not match
}

// Function to print all employees in one line each
void printEmployees() {
    if (globalEmployees == NULL) {
        printf("No employees to display.\n");
        return;
    }

    printf("\n--- Employees List ---\n");

    // Start at the head of the linked list
    Employee* current = globalEmployees;
    while (current != NULL) {
        printf("%s, %s, %s, %s\n",
            current->username,
            current->privateName,
            current->password,
            current->level
        );
        // Move to the next employee in the list
        current = current->next;
    }

    printf("--------------------\n");
}

// Function to print a specific employee by username
void printEmployee(char* username) {
    if (globalEmployees == NULL) {
        printf("No employees to display.\n");
        return;
    }

    // Start at the head of the linked list
    Employee* current = globalEmployees;

    // Iterate over the linked list
    while (current != NULL) {
        // Compare the username
        if (strcmp(current->username, username) == 0) {
            // Username found, print the employee's details
            printf("\n--- Employee Details ---\n");
            printf("Username: %s\n", current->username);
            printf("Private Name: %s\n", current->privateName);
            printf("Password: %s\n", current->password);
            printf("Level: %s\n", current->level);
            printf("-------------------------\n");
            return;
        }
        // Move to the next employee in the list
        current = current->next;
    }

    // If we exit the loop, the username was not found
    printf("Employee with username '%s' not found.\n", username);
}

Employee* getAllEmployees() {
    FILE* fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        printf("Error opening file: %s\n", EMPLOYEES_FILE);
        return NULL;
    }

    Employee* current = NULL;
    Employee* tail = NULL; // Tail keeps track of the last node

    // Read all Employees and insert them into the linked list
    while ((current = readEmployee(fp)) != NULL) {
        // Initialize the next pointer to NULL
        current->next = NULL;

        if (globalEmployees == NULL) {
            // If the list is empty, the first employee becomes the head
            globalEmployees = current;
            tail = globalEmployees;
        }
        else {
            // Otherwise, link the new employee at the end of the list
            tail->next = current;
            tail = current;
        }
    }

    fclose(fp);

    // Return the head of the list, which may be NULL if no employees were read
    return globalEmployees;
}

// Function to sort the linked list by level in ascending order
void sortEmployees() {
    if (globalEmployees == NULL || globalEmployees->next == NULL) {
        // List is empty or has only one element
        return;
    }

    int swapped;
    Employee* current;
    Employee* lastSorted = NULL;

    // Bubble sort implementation
    do {
        swapped = 0;
        current = globalEmployees;

        while (current->next != lastSorted) {
            // Compare levels of the current node and the next node
            if (compareLevels(current->level, current->next->level) > 0) {
                // Swap the nodes' data if they are in the wrong order
                // Swap the contents instead of nodes to keep the linked list intact
                char* tempUsername = current->username;
                char* tempPrivateName = current->privateName;
                char* tempPassword = current->password;
                char* tempLevel = current->level;

                current->username = current->next->username;
                current->privateName = current->next->privateName;
                current->password = current->next->password;
                current->level = current->next->level;

                current->next->username = tempUsername;
                current->next->privateName = tempPrivateName;
                current->next->password = tempPassword;
                current->next->level = tempLevel;

                swapped = 1;
            }
            current = current->next;
        }
        lastSorted = current;
    } while (swapped);

    printf("Employees sorted by level (ascending order).\n");
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

        printf("%d. Return an Item.\n", optionNumber);
        mapping[optionNumber++] = 5;
    }

    // Worker Level (2 and below)
    if (level <= 2) {
        printf("%d. Remove an Item.\n", optionNumber);
        mapping[optionNumber++] = 6;

        printf("%d. Remove a Customer.\n", optionNumber);
        mapping[optionNumber++] = 7;

        printf("%d. Update Item.\n", optionNumber);
        mapping[optionNumber++] = 8;

        printf("%d. Sell an Item.\n", optionNumber);
        mapping[optionNumber++] = 9;
    }

    // Admin Level (1 only)
    if (level == 1) {
        printf("%d. Add New Employee.\n", optionNumber);
        mapping[optionNumber++] = 10;

        printf("%d. Show all Employees.\n", optionNumber);
        mapping[optionNumber++] = 11;
    }

    // Common Options for All Levels
    printf("%d. Show all Items.\n", optionNumber);
    mapping[optionNumber++] = 12;

    printf("%d. Show all Customers.\n", optionNumber);
    mapping[optionNumber++] = 13;

    printf("%d. Exit the RTS System.\n", optionNumber);
    mapping[optionNumber++] = 14;

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
    printf("Created default Admin user!\n");
}

void login() {
    Employee tempEmployee;
    tempEmployee.username = (char*)malloc(USERNAME_LENGTH * sizeof(char));
    tempEmployee.privateName = (char*)malloc(PRIVATE_NAME_LENGTH * sizeof(char));
    tempEmployee.password = (char*)malloc(PASSWORD_LENGTH * sizeof(char));
    tempEmployee.level = (char*)malloc(LEVEL_LENGTH * sizeof(char));

    int user_attempts = 0;
    char* username = (char*)malloc(USERNAME_LENGTH * sizeof(char));
    char* password = (char*)malloc(PASSWORD_LENGTH * sizeof(char));

    assert(username != NULL);
    assert(password != NULL);

    while (user_attempts < ALLOWED_ATTEMPTS) {
        printf("Please enter username: ");
        scanf("%s", username);
        printf("Please enter password: ");
        scanf("%s", password);

        // Load employees, customers, and items 
        globalEmployees = getAllEmployees();

        Employee* tempEmployee = checkCredentials(username, password);

        if (tempEmployee != NULL) {
            // Deep copy each string
            currentEmployee = (Employee*)malloc(sizeof(Employee));
            currentEmployee->username = (char*)malloc(sizeof(char) * USERNAME_LENGTH);
            currentEmployee->privateName = (char*)malloc(sizeof(char) * PRIVATE_NAME_LENGTH);
            currentEmployee->password = (char*)malloc(sizeof(char) * PASSWORD_LENGTH);
            currentEmployee->level = (char*)malloc(sizeof(char) * LEVEL_LENGTH);
            strncpy(currentEmployee->username, tempEmployee->username, USERNAME_LENGTH);
            strncpy(currentEmployee->privateName, tempEmployee->privateName, PRIVATE_NAME_LENGTH);
            strncpy(currentEmployee->password, tempEmployee->password, PASSWORD_LENGTH);
            strncpy(currentEmployee->level, tempEmployee->level, LEVEL_LENGTH);
            currentEmployee->next = NULL;

            free(username);
            free(password);

            globalCustomers = getAllCustomers();
            globalItems = getAllItems();
            return;
        }
        else {

            if (user_attempts == ALLOWED_ATTEMPTS - 1) {
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

void addNewEmployee() {
    clrscr();
    char* username = (char*)malloc(sizeof(char) * USERNAME_LENGTH);
    char* privateName = (char*)malloc(sizeof(char) * PRIVATE_NAME_LENGTH);
    char* password = (char*)malloc(sizeof(char) * PASSWORD_LENGTH);
    char* level = (char*)malloc(sizeof(char) * LEVEL_LENGTH);
    clearBuffer();
    printf("Add New Employee Menu:\n");
    printf("Please enter Username: ");
    getInputString(username, USERNAME_LENGTH);
    printf("Please enter Private Name: ");
    getInputString(privateName, PRIVATE_NAME_LENGTH);
    printf("Please enter Password: ");
    getInputString(password, PASSWORD_LENGTH);
    printf("Please enter level: ");
    getInputString(level , LEVEL_LENGTH);
    Employee* employee = createEmployee(username, privateName, password, level);
    addEmployee(employee);
    printEmployee(employee->username);
    printf("Employee has been added successfully!\n");
}

void showMenu() {
    while (1) {
        int exit = 0;
        int mapping[30] = { 0 };  // Increased array size for mapping

        clrscr();
        int totalOptions = menuItems(atoi(currentEmployee->level), mapping);

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
            //returnItemMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 6: {
            char user;
            clrscr();
            removeItemMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 7: {
            char user;
            clrscr();
            removeCustomerMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 8: {
            char user;
            clrscr();
            updateItemMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 9: {
            char user;
            clrscr();
            sellItemMenu();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 10: {
            char user;
            clrscr();
            addNewEmployee();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 11: {
            char user;
            clrscr();
            printEmployees();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 12: {
            char user;
            int itemCount = 0;
            Item* items = getAllItems(&itemCount);
            printItems();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 13: {
            char user;
            int customersCount;
            printCustomers();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 14:
            exit = 1;
            sortEmployees();
            sortCustomers();
            //sortItems();
            writeEmployees(EMPLOYEES_FILE);
            writeCustomers(CUSTOMERS_FILE);
            //writeItems()
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
        if (exit == 1) {
            break;
        }
    }
}