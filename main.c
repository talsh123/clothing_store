#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "Employee.h"
#include <stdlib.h>
#include <string.h>
#include "StringOperations.h"
#include <assert.h>

Employee currentEmployee;

int main() {
    if (!checkIfEmployeeFileExists()) {
        createDefaultAdmin();
    }

    currentEmployee = login();
    if (currentEmployee.level == 0) {
        printf("Login failed. Try again later..\n");
        return 0;
    }

    showMenu();
    return 0;
}