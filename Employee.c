// Tal-Shalom Ben Ovadia 322356346 
// Stav Moalem 211500657
// Rafi Erez 301420352

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
#include "utils.h"

Employee* createEmployee(char* username, char* privateName, char* password, char* level) {
    // Allocate memory for the Employee structure using safeMalloc.
    Employee* employee = (Employee*)safeMalloc(sizeof(Employee));

    // Allocate and copy the username.
    employee->username = (char*)safeMalloc(USERNAME_LENGTH);
    strncpy(employee->username, username, USERNAME_LENGTH - 1);
    employee->username[USERNAME_LENGTH - 1] = '\0';

    // Allocate and copy the private name.
    employee->privateName = (char*)safeMalloc(PRIVATE_NAME_LENGTH);
    strncpy(employee->privateName, privateName, PRIVATE_NAME_LENGTH - 1);
    employee->privateName[PRIVATE_NAME_LENGTH - 1] = '\0';

    // Allocate and copy the password.
    employee->password = (char*)safeMalloc(PASSWORD_LENGTH);
    strncpy(employee->password, password, PASSWORD_LENGTH - 1);
    employee->password[PASSWORD_LENGTH - 1] = '\0';

    // Allocate and copy the level.
    employee->level = (char*)safeMalloc(LEVEL_LENGTH);
    strncpy(employee->level, level, LEVEL_LENGTH - 1);
    employee->level[LEVEL_LENGTH - 1] = '\0';

    // Log the creation of the new employee.
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Created employee: %s, Level: %s", username, level);
    write_log("l.log", logMessage);

    return employee;
}

Employee* addEmployee(Employee* employee) {
    employee->next = NULL;

    if (globalEmployees == NULL) {
        globalEmployees = employee;
    }
    else {
        Employee* current = globalEmployees;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = employee;
    }

    printf("Employee '%s' added successfully.\n", employee->username);
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Added new Employe with ID: %s, Name: %s", employee->username, employee->privateName);
    write_log("l.log", logMessage);

    return employee;
}

void writeEmployee(Employee* employee, const char* fileName) {
    FILE* file = fopen(fileName, "a");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", fileName);
        write_log("l.log", "Failed to open employee file for writing.");
        return;
    }

    fprintf(file, "%-*s", USERNAME_LENGTH - 1, employee->username);
    fprintf(file, "%-*s", PRIVATE_NAME_LENGTH - 1, employee->privateName);
    fprintf(file, "%-*s", PASSWORD_LENGTH - 1, employee->password);
    fprintf(file, "%-*s\n", LEVEL_LENGTH - 1, employee->level);

    fclose(file);

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Wrote employee %s to file %s", employee->username, fileName);
    write_log("l.log", logMessage);
}


void writeEmployees(const char* fileName) {
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", fileName);
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "Error: Could not open file %s for writing employees.", fileName);
        write_log("l.log", logMessage);
        return;
    }
    fclose(file);

    Employee* current = globalEmployees;
    while (current != NULL) {
        writeEmployee(current, fileName);
        current = current->next;
    }

    printf("All employees have been written to %s successfully.\n", fileName);
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Successfully wrote all employees to file %s.", fileName);
    write_log("l.log", logMessage);
}

Employee* readEmployee(FILE* file) {
    if (file == NULL) {
        printf("Error: Invalid file pointer.\n");
        write_log("l.log", "Invalid file pointer encountered while reading employee.");
        return NULL;
    }

    Employee* employee = (Employee*)safeMalloc(sizeof(Employee));
    employee->username = (char*)safeMalloc(USERNAME_LENGTH);
    employee->privateName = (char*)safeMalloc(PRIVATE_NAME_LENGTH);
    employee->password = (char*)safeMalloc(PASSWORD_LENGTH);
    employee->level = (char*)safeMalloc(LEVEL_LENGTH);

    if (fgets(employee->username, USERNAME_LENGTH, file) == NULL ||
        fgets(employee->privateName, PRIVATE_NAME_LENGTH, file) == NULL ||
        fgets(employee->password, PASSWORD_LENGTH, file) == NULL ||
        fgets(employee->level, LEVEL_LENGTH, file) == NULL) {
        write_log("l.log", "Failed to read employee details from file.");
        free(employee->username);
        free(employee->privateName);
        free(employee->password);
        free(employee->level);
        free(employee);
        return NULL;
    }

    employee->next = NULL;

    int ch;
    while ((ch = fgetc(file)) != '\n' && ch != EOF);

    write_log("l.log", "Employee details successfully read from file.");

    char* temp = trimwhitespace(employee->username);
    free(employee->username);
    employee->username = temp;

    temp = trimwhitespace(employee->privateName);
    free(employee->privateName);
    employee->privateName = temp;

    temp = trimwhitespace(employee->password);
    free(employee->password);
    employee->password = temp;

    temp = trimwhitespace(employee->level);
    free(employee->level);
    employee->level = temp;

    return employee;
}

Employee* checkCredentials(char* username, char* password) {
    Employee* current = globalEmployees;

    while (current != NULL) {
        if (strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) {
            write_log("l.log", "Successful login for user.");
            return current;
        }
        current = current->next;
    }

    printf("Error: Invalid username or password.\n");
    write_log("l.log", "Failed login attempt with invalid credentials.");
    return NULL;
}

void printEmployees() {
    if (globalEmployees == NULL) {
        printf("No employees to display.\n");
        write_log("l.log", "Attempted to print employees but none were available.");
        return;
    }

    printf("\n--- Employees List ---\n");
    printf("%-20s %-20s %-20s %-10s\n", "Username", "Private Name", "Password", "Level");
    printf("--------------------------------------------------------------------------\n");

    Employee* current = globalEmployees;
    while (current != NULL) {
        printf("%-20s %-20s %-20s %-10s\n",
            current->username,
            current->privateName,
            current->password,
            current->level);
        current = current->next;
    }
    printf("--------------------------------------------------------------------------\n");

    write_log("l.log", "Displayed all employees successfully in table format.");
}

// Function to print a specific employee by username
void printEmployee(char* username) {
    if (globalEmployees == NULL) {
        printf("No employees to display.\n");
        write_log("l.log", "No employees available when attempting to print a specific employee.");
        return;
    }

    Employee* current = globalEmployees;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            printf("\n--- Employee Details ---\n");
            printf("%-15s: %s\n", "Username", current->username);
            printf("%-15s: %s\n", "Private Name", current->privateName);
            printf("%-15s: %s\n", "Password", current->password);
            printf("%-15s: %s\n", "Level", current->level);
            printf("-------------------------\n");

            char logMessage[256];
            snprintf(logMessage, sizeof(logMessage), "Displayed details for employee: %s", current->username);
            write_log("l.log", logMessage);
            return;
        }
        current = current->next;
    }

    printf("Employee with username '%s' not found.\n", username);
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Employee with username '%s' not found.", username);
    write_log("printEmployee", logMessage);
}

Employee* getAllEmployees() {
    FILE* fp = fopen(EMPLOYEES_FILE, "r");
    if (fp == NULL) {
        write_log("l.log", "Error opening employees file for reading.");
        return NULL;
    }

    Employee* head = NULL;
    Employee* current = NULL;
    Employee* tail = NULL;
    int count = 0;

    while ((current = readEmployee(fp)) != NULL) {
        current->next = NULL;

        if (count == 0) {
            head = current;
            tail = current;
        }
        else {
            tail->next = current;
            tail = current;
        }
        count++;
    }

    globalEmployees = head;

    fclose(fp);

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Loaded %d employees from file: %s", count, EMPLOYEES_FILE);
    write_log("l.log", logMessage);

    return globalEmployees;
}

void sortEmployees() {
    if (globalEmployees == NULL || globalEmployees->next == NULL) {
        write_log("l.log", "No sorting needed (empty list or only one employee).");
        return;
    }

    int swapped;
    Employee* current;
    Employee* lastSorted = NULL;

    do {
        swapped = 0;
        current = globalEmployees;

        while (current->next != lastSorted) {
            if (compareLevels(current->level, current->next->level) > 0) {
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

    write_log("l.log", "Employees sorted by level (ascending order) successfully.");
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

    printf("%d. Save Changes & Exit the RTS System.\n", optionNumber);
    mapping[optionNumber++] = 14;

    int totalOptions = optionNumber - 1;
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "menuItems: Displayed menu for level %d with %d options.", level, totalOptions);
    write_log("l.log", logMessage);

    return totalOptions;
}

int checkIfEmployeeFileExists() {
    FILE* file = fopen(EMPLOYEES_FILE, "r");
    if (file) {
        fclose(file);
        write_log("l.log", "checkIfEmployeeFileExists: Employee file exists.");
        return 1;
    }
    else {
        write_log("l.log", "checkIfEmployeeFileExists: Employee file does not exist.");
        return 0;
    }
}

void createDefaultAdmin() {
    write_log("l.log", "createDefaultAdmin: Creating default admin user.");
    Employee* employee = createEmployee("admin", "manager", "12345678", "1");
    writeEmployee(employee, EMPLOYEES_FILE);
    printf("No employees.txt file found!\n");
    printf("Created default Admin user!\n");
    write_log("l.log", "createDefaultAdmin: Default admin user created and written to file.");
}

void login() {
    write_log("l.log", "login: Login process started.");

    int user_attempts = 0;
    char* username = (char*)safeMalloc(USERNAME_LENGTH * sizeof(char));
    char* password = (char*)safeMalloc(PASSWORD_LENGTH * sizeof(char));

    while (user_attempts < ALLOWED_ATTEMPTS) {
        printf("Please enter username: ");
        getInputString(username, USERNAME_LENGTH);
        printf("Please enter password: ");
        getInputString(password, PASSWORD_LENGTH);

        // Load employees, customers, and items 
        globalEmployees = getAllEmployees();

        Employee* tempEmployee = checkCredentials(username, password);

        if (tempEmployee != NULL) {
            write_log("l.log", "login: Successful login.");
            currentEmployee = (Employee*)safeMalloc(sizeof(Employee));
            currentEmployee->username = (char*)safeMalloc(sizeof(char) * USERNAME_LENGTH);
            currentEmployee->privateName = (char*)safeMalloc(sizeof(char) * PRIVATE_NAME_LENGTH);
            currentEmployee->password = (char*)safeMalloc(sizeof(char) * PASSWORD_LENGTH);
            currentEmployee->level = (char*)safeMalloc(sizeof(char) * LEVEL_LENGTH);
            strncpy(currentEmployee->username, tempEmployee->username, USERNAME_LENGTH);
            strncpy(currentEmployee->privateName, tempEmployee->privateName, PRIVATE_NAME_LENGTH);
            strncpy(currentEmployee->password, tempEmployee->password, PASSWORD_LENGTH);
            strncpy(currentEmployee->level, tempEmployee->level, LEVEL_LENGTH);
            currentEmployee->next = NULL;

            free(username);
            free(password);

            globalCustomers = getAllCustomers();
            globalItems = loadItemsLinkedList(ITEMS_FILE);
            return;
        }
        else {
            char logMsg[256];
            snprintf(logMsg, sizeof(logMsg), "login: Failed login attempt %d.", user_attempts + 1);
            write_log("l.log", logMsg);

            if (user_attempts == ALLOWED_ATTEMPTS - 1) {
                write_log("l.log", "login: Maximum login attempts reached. Exiting.");
                printf("You have entered incorrect credentials %d times. Please try again later.\n", ALLOWED_ATTEMPTS);
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
    char* username = (char*)safeMalloc(sizeof(char) * USERNAME_LENGTH);
    char* privateName = (char*)safeMalloc(sizeof(char) * PRIVATE_NAME_LENGTH);
    char* password = (char*)safeMalloc(sizeof(char) * PASSWORD_LENGTH);
    char* level = (char*)safeMalloc(sizeof(char) * LEVEL_LENGTH);

    clearBuffer();
    printf("Add New Employee Menu:\n");
    printf("Please enter Username: ");
    getInputString(username, USERNAME_LENGTH);
    printf("Please enter Private Name: ");
    getInputString(privateName, PRIVATE_NAME_LENGTH);
    printf("Please enter Password: ");
    getInputString(password, PASSWORD_LENGTH);
    printf("Please enter level: ");
    getInputString(level, LEVEL_LENGTH);

    Employee* employee = createEmployee(username, privateName, password, level);
    addEmployee(employee);
    printEmployee(employee->username);
    printf("Employee has been added successfully!\n");

    write_log("l.log", "addNewEmployee: New employee added successfully.");

    free(username);
    free(privateName);
    free(password);
    free(level);
}

void showMenu() {
    write_log("l.log", "showMenu: Entered showMenu function.");
    while (1) {
        int exit = 0;
        int mapping[30] = { 0 };  // Array for mapping menu choices

        clrscr();
        int totalOptions = menuItems(atoi(currentEmployee->level), mapping);

        int user_choice;
        printf("Please enter a choice: ");
        scanf("%d", &user_choice);

        // Validate the user choice.
        if (user_choice < 1 || user_choice > totalOptions || mapping[user_choice] == 0) {
            printf("Invalid choice. Please try again.\n");
            char logMsg[256];
            snprintf(logMsg, sizeof(logMsg), "showMenu: Invalid choice entered: %d", user_choice);
            write_log("l.log", logMsg);
            continue;
        }

        {
            char logMsg[256];
            snprintf(logMsg, sizeof(logMsg), "showMenu: User selected option %d (mapped to function %d).", user_choice, mapping[user_choice]);
            write_log("l.log", logMsg);
        }

        // Process the user's menu selection.
        switch (mapping[user_choice]) {
        case 1:
            clrscr();
            viewItems();
            printf("Press Enter to continue...");
            getchar();
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
            returnItemMenu();
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
            clrscr();
            printItems();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 13: {
            char user;
            clrscr();
            printCustomers();
            printf("Press any key to continue! ");
            scanf(" %c", &user);
            clearBuffer();
            break;
        }
        case 14:
            exit = 1;
            sortEmployees(); // works
            sortCustomers(); // works
            writeEmployees(EMPLOYEES_FILE); // works
            writeCustomers(CUSTOMERS_FILE); // works
            saveItemsFromLinkedList(ITEMS_FILE); // works
            write_log("l.log", "showMenu: Exit option selected. Exiting menu.");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            write_log("l.log", "showMenu: Unhandled menu option encountered.");
            break;
        }
        if (exit == 1) {
            break;
        }
    }
    write_log("l.log", "showMenu: Exited menu loop.");
}
