#ifndef EMPLOYEE_H
#define EMPLOYEE_H

typedef struct {
    char* username;
    char* privateName;
    char* password;
    char* level;
} Employee;

#define EMPLOYEES_FILE "employees.txt"

// Employee FILE properties LIMIT
#define USERNAME_LENGTH 64
#define PRIVATE_NAME_LENGTH 64
#define PASSWORD_LENGTH 128
#define LEVEL_LENGTH 2

// User Validation Attemps
#define ALLOWED_ATTEMPS 3

// Extern declaration for global variable
extern Employee currentEmployee;

Employee* createEmployee(char* username, char* privateName, char* password, char* level);
void writeEmployee(Employee* Employee, const char* fileName);
Employee* readEmployee(FILE* file);
Employee* checkCredentials(char* username, char* password);
void printEmployees(Employee* employees, int employeeCount);
Employee* getAllEmployees();
int menuItems(int level, int* mapping);
int checkIfEmployeeFileExists();
void createDefaultAdmin();
Employee login();
void addNewEmployee();
void showMenu();

#endif // EMPLOYEE_H