// Tal-Shalom Ben Ovadia 322356346 
// Stav Moalem 211500657
// Rafi Erez 301420352

#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include "main.h"

typedef struct Employee {
    char* username;
    char* privateName;
    char* password;
    char* level;
    struct Employee* next;  // Pointer to the next employee in the list
} Employee;

#define EMPLOYEES_FILE "employees.txt"

// Employee FILE properties LIMIT
#define USERNAME_LENGTH 64
#define PRIVATE_NAME_LENGTH 64
#define PASSWORD_LENGTH 128
#define LEVEL_LENGTH 2

// User Validation Attempts
#define ALLOWED_ATTEMPTS 3

// Extern declaration for global variable
extern Employee* currentEmployee;  // Points the the employee that is connected

extern Employee* globalEmployees;

Employee* createEmployee(char* username, char* privateName, char* password, char* level);
Employee* addEmployee(Employee* employee);
void writeEmployee(Employee* employee, const char* fileName);
void writeEmployees(const char* fileName);
Employee* readEmployee(FILE* file);
Employee* checkCredentials(char* username, char* password);
void printEmployees();
void printEmployee(char* username);
Employee* getAllEmployees();
void sortEmployees();
int menuItems(int level, int* mapping);
int checkIfEmployeeFileExists();
void createDefaultAdmin();
void login();
void addNewEmployee();
void showMenu();

#endif // EMPLOYEE_H