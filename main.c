#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "Employee.h"
#include <stdlib.h>
#include <string.h>
#include "StringOperations.h"
#include <assert.h>
#include "Customer.h"
#include "Item.h"

// Define the global variables
Employee* currentEmployee = NULL;
Employee* globalEmployees = NULL;
Customer* globalCustomers = NULL;
ItemNode* globalItems = NULL;

int main() {
    if (!checkIfEmployeeFileExists()) {
        createDefaultAdmin();
    }

    login();
    if (currentEmployee->level == 0) {
        printf("Login failed. Try again later..\n");
        return 0;
    }

    showMenu();
    return 0;
}