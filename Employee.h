#ifndef EMPLOYEE_H
#define EMPLOYEE_H

typedef struct {
    char* username;
    char* privateName;
    char* password;
    char* level;
} Employee;

// Extern declaration for global variable
extern Employee currentEmployee;

Employee* createCustomer(char* username, char* privateName, char* password, char* permissionLevel);
void writeEmployee(Employee* Employee, const char* fileName);
Employee* readEmployee(FILE* file);
Employee* checkCredentials(char* username, char* password);
void menuItems(int level);
int checkIfEmployeeFileExists();
void createDefaultAdmin();
Employee login();
void showMenu();

#endif // EMPLOYEE_H