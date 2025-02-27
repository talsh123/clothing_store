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
    FILE* file = fopen(fileName, "a");  // Open in append mode
    if (file == NULL) {
        printf("Error: Unable to open file for writing.\n");
        return;
    }

    // Write each field in a text-friendly format with space as a separator
    fprintf(file, "%-*s ", ID_LENGTH - 1, customer->id);                // ID as string
    fprintf(file, "%-*s ", NAME_LENGTH - 1, customer->name);             // Name as string
    fprintf(file, "%-*s ", JOIN_DATE_LENGTH - 1, customer->joinDate);    // Join date as string
    fprintf(file, "%.2f ", customer->totalAmountSpent);                  // Total amount spent as double with 2 decimal places
    fprintf(file, "%d\n", customer->itemsPurchased);                     // Items purchased as int with newline

    fclose(file);
}

void writeCustomers(Customer* customers, int customerCount, const char* fileName) {
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    // Temporary buffers with the exact lengths specified
    char idBuffer[ID_LENGTH];
    char nameBuffer[NAME_LENGTH];
    char dateBuffer[JOIN_DATE_LENGTH];

    for (int i = 0; i < customerCount; i++) {
        // Copy the strings to temporary buffers, ensuring null-termination
        strncpy(idBuffer, customers[i].id, ID_LENGTH - 1);
        idBuffer[ID_LENGTH - 1] = '\0';

        strncpy(nameBuffer, customers[i].name, NAME_LENGTH - 1);
        nameBuffer[NAME_LENGTH - 1] = '\0';

        strncpy(dateBuffer, customers[i].joinDate, JOIN_DATE_LENGTH - 1);
        dateBuffer[JOIN_DATE_LENGTH - 1] = '\0';

        // Write each field to the file with space separators
        fprintf(file, "%-9s ", idBuffer);    // 9 characters + 1 null
        fprintf(file, "%-63s ", nameBuffer); // 63 characters + 1 null
        fprintf(file, "%-10s ", dateBuffer); // 10 characters + 1 null
        fprintf(file, "%f ", customers[i].totalAmountSpent); // 8 bytes (double)
        fprintf(file, "%d\n", customers[i].itemsPurchased);    // 4 bytes (int) with newline
    }

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

    // Read each field from the text file
    if (fscanf(file, "%9s %63[^\n] %10s %lf %d",
        customer->id,
        customer->name,
        customer->joinDate,
        &(customer->totalAmountSpent),
        &(customer->itemsPurchased)) != 5) {
        free(customer->id);
        free(customer->name);
        free(customer->joinDate);
        free(customer);
        return NULL;
    }

    return customer;
}

// Function to print all employees in one line each
void printCustomers(Customer* customers, int customerCount) {
    if (customers == NULL || customerCount <= 0) {
        printf("No customers to display.\n");
        return;
    }

    printf("\n--- Customers List ---\n");
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

Customer* getAllCustomers(int* customerCount) {
    FILE* fp = fopen(CUSTOMERS_FILE, "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return NULL;
    }

    Customer* customers = NULL;
    *customerCount = 0;
    Customer* tempCustomer;

    // Read all Employees
    while ((tempCustomer = readCustomer(fp)) != NULL) {
        // Dynamically allocate memory
        customers = realloc(customers, sizeof(Customer) * (*customerCount + 1));

        // Hard copy the Item
        customers[*customerCount].id = (char*)malloc(sizeof(char) * ID_LENGTH);
        customers[*customerCount].name = (char*)malloc(sizeof(char) * NAME_LENGTH);
        customers[*customerCount].joinDate = (char*)malloc(sizeof(char) * JOIN_DATE_LENGTH);
        strcpy(customers[*customerCount].id, (*tempCustomer).id);
        strcpy(customers[*customerCount].name, (*tempCustomer).name);
        customers[*customerCount].name = trimwhitespace(customers[*customerCount].name);
        strcpy(customers[*customerCount].joinDate, (*tempCustomer).joinDate);
        customers[*customerCount].totalAmountSpent = tempCustomer->totalAmountSpent;
        customers[*customerCount].itemsPurchased = tempCustomer->itemsPurchased;
        
        // Increment the itemCount
        (*customerCount)++;

        // Check for end of file after attempting to read
        if (feof(fp)) {
            break;
        }
    }

    fclose(fp);
    printCustomers(customers, *customerCount);
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

Customer* findCustomersByProperty(char* property, void* value) {
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
            if (customer->totalAmountSpent == *(double*)value) {
                matches = 1;
            }
        }
        else if (strcmp(property, "items_purchased") == 0) {
            if (customer->itemsPurchased == *(int*)value) {
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

            // Trim white space
            customer->id = trimwhitespace(customer->id);
            customer->name = trimwhitespace(customer->name);
            customer->joinDate = trimwhitespace(customer->joinDate);

            // Deep copy of customer data
            strcpy(matchingCustomers[count - 1].id, customer->id);
            strcpy(matchingCustomers[count - 1].name, customer->name);
            strcpy(matchingCustomers[count - 1].joinDate, customer->joinDate);
            matchingCustomers[count - 1].totalAmountSpent = customer->totalAmountSpent;
            matchingCustomers[count - 1].itemsPurchased = customer->itemsPurchased;
        }

        // Free the memory for the current item
        free(customer->id);
        free(customer->name);
        free(customer->joinDate);
        free(customer);
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
        free(matchingCustomers);
        return NULL;
    }
    else {
        // Also print all the matching Items
        printCustomers(matchingCustomers, count);
    }

    return matchingCustomers;
}

Customer* findCustomersByID(char* userID) {
    FILE* file = fopen(CUSTOMERS_FILE, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", CUSTOMERS_FILE);
        return NULL;
    }

    // Read customers one by one
    Customer* customer;
    int match = 0;
    while ((customer = readCustomer(file)) != NULL) {
        if (strcmp(customer->id, userID) == 0) {
            match = 1;
            break;
        }
        else {
            // Free this customer as it's not a match
            free(customer->id);
            free(customer->name);
            free(customer->joinDate);
            free(customer);
        }
    }

    fclose(file);

    if (match) {
        // Trim white space
        customer->id = trimwhitespace(customer->id);
        customer->name = trimwhitespace(customer->name);
        customer->joinDate = trimwhitespace(customer->joinDate);
        printCustomers(customer, 1);
        return customer;
    }
    else {
        printf("There are no items that matched your search!\n");
        return NULL;
    }
}


Customer* removeCustomer(char* id) {
    int customerCount = 0;
    Customer* customers = getAllCustomers(&customerCount);
    if (customers == NULL || customerCount == 0) {
        printf("No customers to remove.\n");
        return NULL;
    }

    Customer* removedCustomer = NULL;
    int foundIndex = -1;

    // Search for the customer with the given id
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].id, id) == 0) {
            foundIndex = i;
            break;
        }
    }

    // If customer is found
    if (foundIndex != -1) {
        // Allocate memory for the removed customer and copy its data
        removedCustomer = (Customer*)malloc(sizeof(Customer));
        removedCustomer->id = (char*)malloc(sizeof(char) * ID_LENGTH);
        removedCustomer->name = (char*)malloc(sizeof(char) * NAME_LENGTH);
        removedCustomer->joinDate = (char*)malloc(sizeof(char) * JOIN_DATE_LENGTH);

        // Copy customer data to the removed customer
        strcpy(removedCustomer->id, customers[foundIndex].id);
        strcpy(removedCustomer->name, customers[foundIndex].name);
        removedCustomer->name = trimwhitespace(removedCustomer->name);
        strcpy(removedCustomer->joinDate, customers[foundIndex].joinDate);
        removedCustomer->totalAmountSpent = customers[foundIndex].totalAmountSpent;
        removedCustomer->itemsPurchased = customers[foundIndex].itemsPurchased;

        // Shift customers to remove the found item
        for (int i = foundIndex; i < customerCount - 1; i++) {
            customers[i] = customers[i + 1];
        }

        // Reallocate memory to shrink the customers array
        customers = realloc(customers, sizeof(Customer) * (customerCount - 1));
        customerCount--;

        // Update the text file after removal
        writeCustomers(customers, customerCount, CUSTOMERS_FILE);

        printf("Customer with ID %s has been removed.\n", id);
    }
    else {
        printf("Customer with ID %s not found.\n", id);
    }

    // Free the dynamically allocated memory for customers array
    for (int i = 0; i < customerCount; i++) {
        free(customers[i].id);
        free(customers[i].name);
        free(customers[i].joinDate);
    }
    free(customers);

    return removedCustomer;
}

Customer* updateCustomer(char* id, int property, void* value) {
    int customerCount = 0;
    Customer* customers = getAllCustomers(&customerCount);

    if (customers == NULL || customerCount == 0) {
        printf("No customers found.\n");
        return NULL;
    }

    int foundIndex = -1;
    // Find the customer with the given customer id
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].id, id) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("Customer with ID %s not found.\n", id);
        return NULL;
    }

    // Update the specified property
    switch (property) {
    case 1: // id
        strncpy(customers[foundIndex].id, (char*)value, ID_LENGTH - 1);
        customers[foundIndex].id[ID_LENGTH - 1] = '\0'; // Ensure null-termination
        break;
    case 2: // name
        strncpy(customers[foundIndex].name, (char*)value, NAME_LENGTH - 1);
        customers[foundIndex].name[NAME_LENGTH - 1] = '\0'; // Ensure null-termination
        break;
    case 3: // joinDate
        strncpy(customers[foundIndex].joinDate, (char*)value, JOIN_DATE_LENGTH - 1);
        customers[foundIndex].joinDate[JOIN_DATE_LENGTH - 1] = '\0'; // Ensure null-termination
        break;
    case 4: // totalAmountSpent
        customers[foundIndex].totalAmountSpent = *(double*)value;
        break;
    case 5: // itemsPurchased
        customers[foundIndex].itemsPurchased = *(int*)value;
        break;
    default:
        printf("Invalid property.\n");
        return NULL;
    }

    // Save the updated customers back to the file
    writeCustomers(customers, customerCount, CUSTOMERS_FILE);

    printf("Customer with ID %s has been updated.\n", id);

    return &(customers[foundIndex]);
}

void removeCustomerMenu() {
    clrscr();
    printf("Remove Customer Menu:\n");
    printf("Please enter Customer ID: ");
    char* customerID = (char*)malloc(sizeof(char) * ID_LENGTH);
    scanf("%s", customerID);
    removeCustomer(customerID);
}

void addNewCustomer() {
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
    Customer* customer = findCustomersByProperty("id", id);
    printf("Customer has purchased %d items.\n", customer->itemsPurchased);
}