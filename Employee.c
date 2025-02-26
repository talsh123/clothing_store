#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Employee.h"
#include "Customer.h"
#include "Item.h"
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

void menuItems(int level) {
    printf("Welcome to the RTS System!\n");
    printf("Menu:\n");
    if (level <= 3) {
        printf("1. View Items.\n");
        printf("2. Add New Item.\n");
        printf("3. Add New Customer.\n");
        printf("4. View Customer Purchases.\n");
    }
    if (level <= 2) {
        printf("5. Remove an Item.\n");
        printf("6. Remove a Customer.\n");
        printf("7. Update Item.\n");
        printf("8. Sell an Item.\n");
    }
    if (level == 1) {
        printf("9. Add New Employee.\n");
    }
    printf("10. Show all Items.\n");
    printf("11. Show all Employees.\n");
    printf("12. Show all Customers.\n");
    printf("13. Exit the RTS System.\n");
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
    Employee* employee = createCustomer("admin", "manager", "12345678", "1");
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
                printf("You have entered incorrect credentials. Please try again!\n");
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
    Employee* employee = createCustomer(username, privateName, password, level);
    writeEmployee(employee, EMPLOYEES_FILE);
    printItems(employee, 1);
    printf("Employee has been added successfully!\n");
}

void showMenu() {
    while (1) {
        int exit = 0;
        clrscr();
        menuItems(atoi(currentEmployee.level));
        int user_choice;
        clearBuffer();
        printf("please enter a choice: ");
        scanf("%d", &user_choice);
        switch (user_choice) {
        case 1:
            clrscr();
            viewItems();
            break;
        case 2:
            clrscr();
            addNewItem();
            break;
        case 3:
            clrscr();
            addNewCustomer();
            break;
        case 4: {
            char user;
            clrscr();
            checkCustomerPurchases();
            printf("Press any key to continue! ");
            scanf("%c", &user);
            clearBuffer();
            break;
        }
        case 5: {
            char user;
            clrscr();
            removeItemMenu();
            printf("Press any key to continue! ");
            scanf("%c", &user);
            clearBuffer();
            break;
        }
        case 9: {
            char user;
            addNewEmployee();
            printf("Press any key to continue! ");
            scanf("%c", &user);
            clearBuffer();
            break;
        }
        case 10: {
            char user;
            int itemCount = 0;
            getAllItems(&itemCount);
            printf("Press any key to continue! ");
            scanf("%c", &user);
            clearBuffer();
            break;
        }
        case 11: {
            char user;
            getAllEmployees();
            printf("Press any key to continue! ");
            scanf("%c", &user);
            clearBuffer();
            break;
        }
        case 12: {
            char user;
            getAllCustomers();
            printf("Press any key to continue! ");
            scanf("%c", &user);
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