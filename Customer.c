// Tal-Shalom Ben Ovadia 322356346 
// Stav Moalem 211500657
// Rafi Erez 301420352

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Customer.h"
#include "Item.h"
#include "StringOperations.h"
#include <assert.h>
#include <time.h>
#include "main.h"
#include "utils.h"

Customer* createCustomer(char* id, char* name) {
    Customer* customer = (Customer*)malloc(sizeof(Customer));
    if (!customer) {
        printf("Error: Memory allocation failed for new customer.\n");
        write_log("l.log", "Failed to allocate memory for new customer.");
        return NULL;
    }

    customer->id = (char*)malloc(sizeof(char) * ID_LENGTH);
    customer->name = (char*)malloc(sizeof(char) * NAME_LENGTH);
    customer->joinDate = (char*)malloc(sizeof(char) * JOIN_DATE_LENGTH);

    if (!customer->id || !customer->name || !customer->joinDate) {
        printf("Error: Memory allocation failed for customer fields.\n");
        write_log("l.log", "Failed to allocate memory for customer fields.");
        free(customer->id);
        free(customer->name);
        free(customer->joinDate);
        free(customer);
        return NULL;
    }

    strncpy(customer->id, id, ID_LENGTH - 1);
    customer->id[ID_LENGTH - 1] = '\0';
    customer->id = trimwhitespace(customer->id);
    strncpy(customer->name, name, NAME_LENGTH - 1);
    customer->name[NAME_LENGTH - 1] = '\0';
    customer->name = trimwhitespace(customer->name);
    strncpy(customer->joinDate, getCurrentDate(), JOIN_DATE_LENGTH);

    customer->totalAmountSpent = 0.0;
    customer->itemsPurchased = 0;
    customer->purchaseCount = 0;
    customer->purchases = NULL;
    customer->next = NULL;

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Created new customer with ID: %s, Name: %s", id, name);
    write_log("l.log", logMessage);

    return customer;
}

Customer* addCustomer(Customer* customer) {
    customer->next = NULL;

    if (globalCustomers == NULL) {
        globalCustomers = customer;
    }
    else {
        Customer* current = globalCustomers;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = customer;
    }

    printf("Customer '%s' added successfully.\n", customer->name);
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Added customer with ID: %s, Name: %s", customer->id, customer->name);
    write_log("l.log", logMessage);

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
    fprintf(file, "%d ", customer->itemsPurchased);                     // Items purchased as int

    // Write Purchase Count
    fprintf(file, "%d ", customer->purchaseCount);

    // Write Purchase Details
    if (customer->purchases != NULL)
        for (int i = 0; i < customer->purchaseCount; i++) {
            fprintf(file, "%-*s ", SERIAL_NUMBER_LENGTH - 1, customer->purchases[i].serialNumber);
            fprintf(file, "%d ", customer->purchases[i].amount);
            fprintf(file, "%-*s ", PURCHASE_DATE_LENGTH - 1, customer->purchases[i].purchaseDate);
        }

    // End line for this customer
    fprintf(file, "\n");

    fclose(file);
}

void writeCustomers(const char* fileName) {
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        write_log("l.log", "Failed to open customer file for writing.");
        return;
    }

    Customer* current = globalCustomers;
    while (current != NULL) {
        writeCustomer(current, CUSTOMERS_FILE);
        current = current->next;
    }

    fclose(file);
    write_log("l.log", "Successfully wrote customers to file.");
}

Customer* readCustomer(FILE* file) {
    if (file == NULL) {
        printf("Error: File pointer is NULL.\n");
        return NULL;
    }

    // Check for end of file after attempting to read
    if (feof(file)) {
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

    if (customer->id == NULL || customer->name == NULL || customer->joinDate == NULL) {
        printf("Error: Memory allocation failed for strings.\n");
        free(customer);
        return NULL;
    }

    // Read Basic Customer Information
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

    // Read Purchase Count
    int purchaseCount;
    if (fscanf(file, "%d", &purchaseCount) == 1 && purchaseCount > 0) {
        customer->purchaseCount = purchaseCount;
        customer->purchases = (Purchase*)malloc(sizeof(Purchase) * purchaseCount);

        for (int i = 0; i < purchaseCount; i++) {
            customer->purchases[i].serialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
            customer->purchases[i].purchaseDate = (char*)malloc(sizeof(char) * PURCHASE_DATE_LENGTH);

            if (fscanf(file, "%12s %d %10s",
                customer->purchases[i].serialNumber,
                &(customer->purchases[i].amount),
                customer->purchases[i].purchaseDate) != 3) {
                printf("Error: Failed to read purchase details.\n");
                break;
            }
        }
    }
    else {
        customer->purchaseCount = 0;
        customer->purchases = NULL;
    }

    return customer;
}

// Function to print all customers in one line each, including purchase history
void printCustomers() {
    if (globalCustomers == NULL) {
        printf("No customers to display.\n");
        write_log("l.log", "Attempted to print customers but list is empty.");
        return;
    }

    printf("\n--- Customers List ---\n");
    Customer* current = globalCustomers;
    while (current != NULL) {
        // Print basic customer details
        printf("Customer ID: %s, Name: %s, Join Date: %s, Total Amount Spent: %.2f, Items Purchased: %d\n",
            current->id, current->name, current->joinDate, current->totalAmountSpent, current->itemsPurchased);

        // Print purchase history if available
        if (current->purchaseCount > 0 && current->purchases != NULL) {
            printf("  Purchases:\n");
            for (int i = 0; i < current->purchaseCount; i++) {
                printf("    [%d] Serial Number: %s, Amount: %d, Purchase Date: %s\n",
                    i + 1,
                    current->purchases[i].serialNumber,
                    current->purchases[i].amount,
                    current->purchases[i].purchaseDate
                );
            }
        }
        else {
            printf("  No Purchases\n");
        }

        current = current->next;
    }
    printf("--------------------\n");
    write_log("l.log", "Printed all customers.");
}

// Function to print a specific customer by ID, including purchase history
void printCustomer(char* id) {
    Customer* current = globalCustomers;
    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            // Print basic customer details
            printf("\n--- Customer Details ---\n");
            printf("Customer ID: %s\nName: %s\nJoin Date: %s\nTotal Amount Spent: %.2f\nItems Purchased: %d\n",
                current->id, current->name, current->joinDate, current->totalAmountSpent, current->itemsPurchased);

            // Print purchase history if available
            if (current->purchaseCount > 0 && current->purchases != NULL) {
                printf("  Purchases:\n");
                for (int i = 0; i < current->purchaseCount; i++) {
                    printf("    [%d] Serial Number: %s, Amount: %d, Purchase Date: %s\n",
                        i + 1,
                        current->purchases[i].serialNumber,
                        current->purchases[i].amount,
                        current->purchases[i].purchaseDate
                    );
                }
            }
            else {
                printf("  No Purchases\n");
            }

            printf("-------------------------\n");

            char logMessage[256];
            snprintf(logMessage, sizeof(logMessage), "Printed details for customer ID: %s", id);
            write_log("l.log", logMessage);
            return;
        }
        current = current->next;
    }

    // If customer not found
    printf("Customer with ID %s not found.\n", id);
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Failed to print details for customer ID: %s - Not found", id);
    write_log("l.log", logMessage);
}


Customer* getAllCustomers() {
    FILE* fp = fopen(CUSTOMERS_FILE, "r");
    if (fp == NULL) {
        write_log("l.log", "Error opening customers file for reading.");
        return NULL;
    }

    Customer* head = NULL;
    Customer* current = NULL;
    Customer* tail = NULL;
    int count = 0;

    while ((current = readCustomer(fp)) != NULL) {
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

    globalCustomers = head;

    fclose(fp);

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Loaded %d customers from file: %s", count, CUSTOMERS_FILE);
    write_log("l.log", logMessage);

    return globalCustomers;
}

// Function to sort the linked list by joinDate in descending order
void sortCustomers() {
    if (globalCustomers == NULL || globalCustomers->next == NULL) {
        // List is empty or has only one element
        return;
    }

    int swapped;
    Customer* current;
    Customer* lastSorted = NULL;

    // Bubble sort implementation
    do {
        swapped = 0;
        current = globalCustomers;

        while (current->next != lastSorted) {
            // Compare join dates of the current node and the next node
            // Descending order, so check if current date is earlier
            if (compareDates(current->joinDate, current->next->joinDate) < 0) {
                // Swap the nodes' data if they are in the wrong order
                // Swap the contents instead of nodes to keep the linked list intact
                char* tempId = current->id;
                char* tempName = current->name;
                char* tempJoinDate = current->joinDate;
                double tempTotalAmountSpent = current->totalAmountSpent;
                int tempItemsPurchased = current->itemsPurchased;
                int tempPurchaseCount = current->purchaseCount;
                Purchase* tempPurchases = current->purchases;

                current->id = current->next->id;
                current->name = current->next->name;
                current->joinDate = current->next->joinDate;
                current->totalAmountSpent = current->next->totalAmountSpent;
                current->itemsPurchased = current->next->itemsPurchased;
                current->purchaseCount = current->next->purchaseCount;
                current->purchases = current->next->purchases;

                current->next->id = tempId;
                current->next->name = tempName;
                current->next->joinDate = tempJoinDate;
                current->next->totalAmountSpent = tempTotalAmountSpent;
                current->next->itemsPurchased = tempItemsPurchased;
                current->next->purchaseCount = tempPurchaseCount;
                current->next->purchases = tempPurchases;

                swapped = 1;
            }
            current = current->next;
        }
        lastSorted = current;
    } while (swapped);

    printf("Customers sorted by join date (latest first).\n");
}

// This function allows the user to get all customers with a certain property
// These properties are:
// id
// name
// joinDate
// totalAmountSpent
// itemsPurchased
// purchaseCount
// NOTE: The user cannot find a user based on the user's purchased items!
Customer* findCustomersByProperty(char* property, void* value) {
    Customer* current = globalCustomers;
    while (current != NULL) {
        int matches = 0;
        if (strcmp(property, "id") == 0 && strcmp(current->id, (char*)value) == 0) {
            matches = 1;
        }
        else if (strcmp(property, "name") == 0 && strcmp(current->name, (char*)value) == 0) {
            matches = 1;
        }
        else if (strcmp(property, "joinDate") == 0 && strcmp(current->joinDate, (char*)value) == 0) {
            matches = 1;
        }
        else if (strcmp(property, "total_amount_spent") == 0 && current->totalAmountSpent == *(double*)value) {
            matches = 1;
        }
        else if (strcmp(property, "items_purchased") == 0 && current->itemsPurchased == *(int*)value) {
            matches = 1;
        }

        if (matches) {
            char logMessage[256];
            snprintf(logMessage, sizeof(logMessage), "Found customer with %s: %s", property, (char*)value);
            write_log("l.log", logMessage);
            return current;
        }
        current = current->next;
    }
    printf("No customer found with %s matching value.\n", property);
    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "No customer found with %s: %s", property, (char*)value);
    write_log("l.log", logMessage);
    return NULL;
}

Customer* removeCustomer(char* id) {
    Customer* current = globalCustomers;
    Customer* prev = NULL;
    Customer* removed = NULL;

    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            removed = current;

            if (prev == NULL) {
                globalCustomers = current->next;
            }
            else {
                prev->next = current->next;
            }

            removed->next = NULL;

            free(removed->id);
            free(removed->name);
            free(removed->joinDate);

            if (removed->purchases != NULL) {
                for (int i = 0; i < removed->purchaseCount; i++) {
                    free(removed->purchases[i].serialNumber);
                    free(removed->purchases[i].purchaseDate);
                }
                free(removed->purchases);
            }

            free(removed);

            printf("Customer with ID %s has been removed.\n", id);
            return globalCustomers;
        }

        // Move to the next node
        prev = current;
        current = current->next;
    }

    // If no node with the matching ID was found
    printf("Customer with ID %s not found.\n", id);
    return globalCustomers;
}

Customer* updateCustomer(char* id, char* property, void* value) {
    Customer* head = globalCustomers;
    Customer* current = head;

    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            char logMessage[256];

            if (strcmp(property, "id") == 0) {
                char* trimmedID = trimwhitespace((char*)value);
                current->id = (char*)realloc(current->id, strlen(trimmedID) + 1);
                if (current->id == NULL) {
                    printf("Error: Memory allocation failed for id.\n");
                    return NULL;
                }
                strcpy(current->id, trimmedID);
                snprintf(logMessage, sizeof(logMessage), "Updated customer ID to: %s", trimmedID);
            }
            else if (strcmp(property, "name") == 0) {
                char* trimmedName = trimwhitespace((char*)value);
                current->name = (char*)realloc(current->name, strlen(trimmedName) + 1);
                if (current->name == NULL) {
                    printf("Error: Memory allocation failed for name.\n");
                    return NULL;
                }
                strcpy(current->name, trimmedName);
                snprintf(logMessage, sizeof(logMessage), "Updated customer Name to: %s", trimmedName);
            }
            else if (strcmp(property, "total_amount_spent") == 0) {
                current->totalAmountSpent = *(double*)value;
                snprintf(logMessage, sizeof(logMessage), "Updated customer total amount spent to: %.2f", *(double*)value);
            }
            else if (strcmp(property, "items_purchased") == 0) {
                current->itemsPurchased = *(int*)value;
                snprintf(logMessage, sizeof(logMessage), "Updated customer items purchased to: %d", *(int*)value);
            }
            else {
                printf("Invalid property: %s\n", property);
                return NULL;
            }
            write_log("l.log", logMessage);
            printf("Customer with ID %s has been updated.\n", id);
            return current;
        }
        current = current->next;
    }
    printf("Customer with ID %s not found.\n", id);
    return NULL;
}

// another option for sorting
//void sortCustomers() {
//    if (globalCustomers == NULL || globalCustomers->next == NULL) {
//        return;
//    }
//
//    int swapped;
//    Customer** ptr;
//    Customer* temp;
//    Customer* lastSorted = NULL;
//
//    do {
//        swapped = 0;
//        ptr = &globalCustomers;
//
//        while ((*ptr)->next != lastSorted) {
//            if (strcmp((*ptr)->joinDate, (*ptr)->next->joinDate) > 0) {
//                temp = *ptr;
//                *ptr = (*ptr)->next;
//                temp->next = (*ptr)->next;
//                (*ptr)->next = temp;
//                swapped = 1;
//            }
//            ptr = &((*ptr)->next);
//        }
//        lastSorted = *ptr;
//    } while (swapped);
//
//    printf("Customers sorted by join date (oldest first).\n");
//    write_log("l.log", "Sorted customers by join date.");
//}


void removeCustomerMenu() {
    clrscr();
    printf("Remove Customer Menu:\n");

    char* id = (char*)malloc(sizeof(char) * ID_LENGTH);
    printf("Please enter Customer ID: ");
    clearBuffer();
    getInputString(id, ID_LENGTH);

    Customer* removedCustomer = removeCustomer(id);
    if (removedCustomer != NULL) {
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "Removed customer with ID: %s", id);
        write_log("l.log", logMessage);
    }
    else {
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "Attempted to remove customer with ID: %s but customer not found.", id);
        write_log("l.log", logMessage);
    }
}


void addNewCustomer() {
    char* id = (char*)malloc(sizeof(char) * ID_LENGTH);
    char* name = (char*)malloc(sizeof(char) * NAME_LENGTH);
    printf("Add Customer Menu:\n");
    printf("Please enter Customer ID: ");
    clearBuffer();
    getInputString(id, ID_LENGTH);

    printf("Please enter Customer Name: ");
    getInputString(name, NAME_LENGTH);

    Customer* customer = createCustomer(id, name);
    addCustomer(customer);

    printCustomers();
    printf("Customer has been added successfully!\n");

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Added new customer with ID: %s, Name: %s", id, name);
    write_log("l.log", logMessage);
}

void checkCustomerPurchases() {
    char* id = (char*)malloc(sizeof(char) * ID_LENGTH);
    printf("Check Customer Purchases Menu:\n");
    printf("Please enter Customer ID: ");
    clearBuffer();
    getInputString(id, ID_LENGTH);
    Customer* customer = findCustomersByProperty("id", id);
    if (customer != NULL) {
        printCustomer(id);
        // Log successful lookup and display of customer purchases.
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "Checked purchases for customer with ID %s", id);
        write_log("l.log", logMessage);
    }
    else {
        printf("Customer not found.\n");
        // Log the failure to find the customer.
        char logMessage[256];
        snprintf(logMessage, sizeof(logMessage), "Customer with ID %s not found during purchase check", id);
        write_log("l.log", logMessage);
    }
}