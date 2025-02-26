#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Customer.h"
#include "Item.h"
#include "FileOperations.h"
#include "StringOperations.h"
#include <assert.h>
#include <time.h>

#define CUSTOMERS_FILE "customers.txt"

// Employee FILE properties LIMIT
#define ID_LENGTH 10 // 9 + 1
#define NAME_LENGTH 64 
#define JOIN_DATE_LENGTH 11 //DD-MM-YYYY + 1
#define TOTAL_AMOUNT_SPENT_LENGTH 8 // Double
#define ITEMS_PURCHASED_LENGTH 4 // Integer

Customer* createCustomer(char* id, char* name, char* joinDate, double totalAmountSpent, int itemsPurchased) {
    Customer* customer = (Customer*)malloc(sizeof(Customer));

    customer->id = (char*)malloc(sizeof(char) * ID_LENGTH);
    strncpy(customer->id, id, strlen(id));
    customer->id[strlen(id)] = '\0'; // Ensure null termination

    customer->name = (char*)malloc(sizeof(char) * NAME_LENGTH);
    strncpy(customer->name, name, strlen(name));
    customer->name[strlen(name)] = '\0'; // Ensure null termination

    customer->joinDate = (char*)malloc(JOIN_DATE_LENGTH);
    strncpy(customer->joinDate, joinDate, strlen(joinDate));
    customer->joinDate[strlen(joinDate)] = '\0'; // Ensure null termination

    customer->totalAmountSpent = totalAmountSpent;
    customer->itemsPurchased = itemsPurchased;

    return customer;
}

void writeCustomer(Customer* customer, const char* fileName) {
    FILE* file = fopen(fileName, "a");
    if (file == NULL) {
        printf("Error: Unable to open file for writing.\n");
        return;
    }

    // Write Customer fields with space as separator
    fprintf(file, "%-*s", ID_LENGTH, customer->id);                // ID as string
    fprintf(file, "%-*s", NAME_LENGTH, customer->name);             // Name as string
    fprintf(file, "%-*s", JOIN_DATE_LENGTH, customer->joinDate);    // Join date as string
    fprintf(file, "%lf", customer->totalAmountSpent);             // Total amount spent as double
    fprintf(file, "%d\n", customer->itemsPurchased);                 // Items purchased as int

    fclose(file);
}

Customer* readCustomer(FILE* file) {
    if (file == NULL) {
        printf("Error: File pointer is NULL.\n");
        return NULL;
    }

    // Allocate memory for Customer
    Customer* customer = (Customer*)malloc(sizeof(Customer));
    if (customer == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    // Allocate memory for strings
    customer->id = (char*)malloc(sizeof(char) * ID_LENGTH);
    customer->name = (char*)malloc(sizeof(char) * NAME_LENGTH);
    customer->joinDate = (char*)malloc(sizeof(char) * JOIN_DATE_LENGTH);

    // Read fixed-length fields from the file
    fread(customer->id, sizeof(char), ID_LENGTH, file);
    customer->id[ID_LENGTH - 1] = '\0'; // Ensure null-termination
    char* trimmed_customer_id = trimwhitespace(customer->id);
    customer->id = trimmed_customer_id;

    fread(customer->name, sizeof(char), NAME_LENGTH, file);
    customer->name[NAME_LENGTH - 1] = '\0'; // Ensure null-termination
    char* trimmed_customer_name = trimwhitespace(customer->name);
    customer->name = trimmed_customer_name;

    fread(customer->joinDate, sizeof(char), JOIN_DATE_LENGTH, file);
    customer->joinDate[JOIN_DATE_LENGTH - 1] = '\0'; // Ensure null-termination
    char* trimmed_customer_join_date = trimwhitespace(customer->joinDate);
    customer->joinDate = trimmed_customer_join_date;

    fscanf(file, "%lf %d", &(customer->totalAmountSpent), &(customer->itemsPurchased));

    // Check for end of file or error
    if (feof(file) || ferror(file)) {
        // Return the customer you've just read
        return customer;
    }

    return customer;
}

// Function to print all employees in one line each
void printCustomers(Customer* customers, int customerCount) {
    if (customers == NULL || customerCount <= 0) {
        printf("No employees to display.\n");
        return;
    }

    printf("\n--- Employees List ---\n");
    for (int i = 0; i < customerCount; i++) {
        printf("%s, %s, %s, %.2lf, %d\n",
            customers[i].id,
            customers[i].name,
            customers[i].joinDate,
            customers[i].totalAmountSpent,
            customers[i].itemsPurchased
        );
    }
    printf("--------------------\n");
}

Customer* getAllCustomers() {
    FILE* fp = fopen(CUSTOMERS_FILE, "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return NULL;
    }

    Customer* customers = NULL;
    int customerCount = 0;
    Customer* tempCustomer;

    // Read all Employees
    while ((tempCustomer = readCustomer(fp)) != NULL) {
        // Dynamically allocate memory
        customers = realloc(customers, sizeof(Customer) * (customerCount + 1));

        // Hard copy the Item
        customers[customerCount].id = (char*)malloc(sizeof(char) * ID_LENGTH);
        customers[customerCount].name = (char*)malloc(sizeof(char) * NAME_LENGTH);
        customers[customerCount].joinDate = (char*)malloc(sizeof(char) * JOIN_DATE_LENGTH);
        strcpy(customers[customerCount].id, (*tempCustomer).id);
        strcpy(customers[customerCount].name, (*tempCustomer).name);
        strcpy(customers[customerCount].joinDate, (*tempCustomer).joinDate);
        customers[customerCount].totalAmountSpent = tempCustomer->totalAmountSpent;
        customers[customerCount].itemsPurchased = tempCustomer->itemsPurchased;
        
        // Increment the itemCount
        customerCount++;

        // Check for end of file after attempting to read
        if (feof(fp)) {
            break;
        }
    }

    fclose(fp);
    printCustomers(customers, customerCount);
    return customers;
}

int checkIfCustomerFileExists() {
    FILE* file;
    if ((file = fopen(CUSTOMERS_FILE, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}

Customer* findUserByProperty(char* property, void* value) {
    FILE* file = fopen(CUSTOMERS_FILE, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", CUSTOMERS_FILE);
        return NULL;
    }

    // Dynamic array to store matching customers
    Customer* matchingCustomers = NULL;

    // Read items one by one
    Customer* customer;
    int count = 0;
    while ((customer = readCustomer(file)) != NULL) {

        int matches = 0;

        // Check property and compare value
        if (strcmp(property, "id") == 0) {
            if (strcmp(customer->id, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "name") == 0) {
            if (strcmp(customer->name, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "join_date") == 0) {
            if (strcmp(customer->joinDate, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "total_amount_spent") == 0) {
            if (customer->joinDate == (double*)value) {
                matches = 1;
            }
        }
        else if (strcmp(property, "items_purchased") == 0) {
            if (customer->itemsPurchased == (int*)value) {
                matches = 1;
            }
        }
        else {
            printf("Invalid property: %s\n", property);
            fclose(file);
            return NULL;
        }
        // If item matches, add it to the dynamic array
        if (matches) {
            count++;
            // Resize array to hold another item
            Customer* tempArray = realloc(matchingCustomers, sizeof(Customer) * count);
            if (tempArray == NULL) {
                printf("Error: Memory allocation failed.\n");
                fclose(file);
                return NULL;
            }
            matchingCustomers = tempArray;
            matchingCustomers[count - 1] = *customer;
        }

        // Check for end of file after attempting to read
        if (feof(file)) {
            break;
        }

        // Free the memory for the current item
        free(customer);
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
    }
    // Print all the matching Items
    else {
        // Also print all the matching Items
        printCustomers(matchingCustomers, count);
    }
    return matchingCustomers;
}

void addNewCustomer() {
    clrscr();
    char* id = (char*)malloc(sizeof(char) * ID_LENGTH);
    char* name = (char*)malloc(sizeof(char) * NAME_LENGTH);
    char* joinDate = (char*)malloc(sizeof(char) * JOIN_DATE_LENGTH);
    float totalAmountSpent = 0.0;
    int itemsPurchased = 0;
    clearBuffer();
    printf("Add Customer Menu:\n");
    printf("Please enter Customer ID: ");
    scanf("%s", id);
    clearBuffer();
    printf("Please enter Customer Name: ");
    fgets(name, NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0;
    // Get the current time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // Format the date as DD-MM-YYYY
    char date[11];
    sprintf(date, "%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    joinDate = date;
    Customer* customer = createCustomer(id, name, joinDate, totalAmountSpent, itemsPurchased);
    writeCustomer(customer, CUSTOMERS_FILE);
    printCustomers(customer , 1);
    printf("Customer has been added successfully!\n");
}

void checkCustomerPurchases() {
    char* id = (char*)malloc(sizeof(char) * ID_LENGTH);
    printf("Check Customer Purchases Menu:\n");
    printf("Please enter Customer ID: ");
    scanf("%s", id);
    Customer* customer = findUserByProperty("id", id);
    printf("Customer has purchased %d items.\n", customer->itemsPurchased);
}