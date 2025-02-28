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

Customer* createCustomer(char* id, char* name) {
    Customer* customer = (Customer*)malloc(sizeof(Customer));

    // ID
    customer->id = (char*)malloc(sizeof(char) * ID_LENGTH);
    strncpy(customer->id, id, ID_LENGTH - 1);
    customer->id[ID_LENGTH - 1] = '\0'; // Ensure null termination

    // Name
    customer->name = (char*)malloc(sizeof(char) * NAME_LENGTH);
    strncpy(customer->name, name, NAME_LENGTH - 1);
    customer->name[NAME_LENGTH - 1] = '\0'; // Ensure null termination

    // Join Date
    customer->joinDate = (char*)malloc(sizeof(char) * JOIN_DATE_LENGTH);
    strncpy(customer->joinDate, getCurrentDate(), JOIN_DATE_LENGTH);

    // Total Amount Spent
    customer->totalAmountSpent = 0.0;

    // Items Purchased
    customer->itemsPurchased = 0;

    // Purchase Count
    customer->purchaseCount = 0;

    // Purchases
    customer->purchases = NULL;;

    // Next Pointer
    customer->next = NULL;

    return customer;
}

Customer* addCustomer(Customer* customer) {
    // Initialize the next pointer of the new customer to NULL
    customer->next = NULL;

    // If the list is empty, the new customer becomes the head
    if (globalCustomers == NULL) {
        globalCustomers = customer;
    }
    else {
        // Otherwise, traverse to the end of the list
        Customer* current = globalCustomers;
        while (current->next != NULL) {
            current = current->next;
        }
        // Add the new customer at the end of the list
        current->next = customer;
    }

    printf("Customer '%s' added successfully.\n", customer->name);

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
    // Open the file in write mode to clear its contents
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    fclose(file);

    Customer* current = globalCustomers;
    while (current != NULL) {
        writeCustomer(current, fileName);
        current = current->next;
    }
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
        return;
    }

    printf("\n--- Customers List ---\n");
    Customer* current = globalCustomers;
    while (current != NULL) {
        // ????? ???? ?????
        printf("%s, %s, %s, %.2lf, %d, %d",
            current->id,
            current->name,
            current->joinDate,
            current->totalAmountSpent,
            current->itemsPurchased,
            current->purchaseCount
        );

        // ????? ???????? ?????? ?? ?????
        if (current->purchaseCount > 0 && current->purchases != NULL) {
            printf(" | Purchases: ");
            for (int j = 0; j < current->purchaseCount; j++) {
                printf("{Serial: %s, Amount: %d, Date: %s}",
                    current->purchases[j].serialNumber,
                    current->purchases[j].amount,
                    current->purchases[j].purchaseDate
                );
                if (j < current->purchaseCount - 1) {
                    printf(", ");
                }
            }
        }
        else {
            printf(" | No Purchases");
        }
        printf("\n");
        current = current->next;
    }
    printf("--------------------\n");
}

void printCustomer(char* id) {
    if (globalCustomers == NULL) {
        printf("No customers to display.\n");
        return;
    }

    // Start at the head of the linked list
    Customer* current = globalCustomers;

    // Iterate over the linked list
    while (current != NULL) {
        // Compare the ID
        if (strcmp(current->id, id) == 0) {
            // ID found, print the customer's details
            printf("\n--- Customer Details ---\n");
            printf("%s, %s, %s, %.2lf, %d, %d",
                current->id,
                current->name,
                current->joinDate,
                current->totalAmountSpent,
                current->itemsPurchased,
                current->purchaseCount
            );

            // Print purchase history if available
            if (current->purchaseCount > 0 && current->purchases != NULL) {
                printf(" | Purchases: ");
                for (int j = 0; j < current->purchaseCount; j++) {
                    printf("{Serial: %s, Amount: %d, Date: %s}",
                        current->purchases[j].serialNumber,
                        current->purchases[j].amount,
                        current->purchases[j].purchaseDate
                    );
                    if (j < current->purchaseCount - 1) {
                        printf(", ");
                    }
                }
            }
            else {
                printf(" | No Purchases");
            }
            printf("\n------------------------\n");
            return;
        }
        // Move to the next customer in the list
        current = current->next;
    }

    // If we exit the loop, the ID was not found
    printf("Customer with ID '%s' not found.\n", id);
}

Customer* getAllCustomers() {
    FILE* fp = fopen(CUSTOMERS_FILE, "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        printf("There are no customer yet, returning empty linked list!\n");
        return NULL;
    }

    Customer* head = NULL;
    Customer* tail = NULL;
    Customer* tempCustomer;

    while ((tempCustomer = readCustomer(fp)) != NULL) {
        if (head == NULL) {
            head = tail = tempCustomer;
        }
        else {
            tail->next = tempCustomer;
            tail = tempCustomer;
        }
    }

    fclose(fp);
    return head;
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
    Customer* head = getAllCustomers();
    Customer* current = head;
    while (current != NULL) {
        int matches = 0;
        if (strcmp(property, "id") == 0) {
            if (strcmp(current->id, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "name") == 0) {
            if (strcmp(current->name, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "join_date") == 0) {
            if (strcmp(current->joinDate, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "total_amount_spent") == 0) {
            if (current->totalAmountSpent == *(double*)value) {
                matches = 1;
            }
        }
        else if (strcmp(property, "items_purchased") == 0) {
            if (current->itemsPurchased == *(int*)value) {
                matches = 1;
            }
        }
        else if (strcmp(property, "purchase_count") == 0) {
            if (current->purchaseCount == *(int*)value) {
                matches = 1;
            }
        }
        else {
            printf("Invalid property: %s\n", property);
            return NULL;
        }

        if (matches) {
            return current;
        }

        current = current->next;
    }

    printf("There are no customers that matched your search!\n");
    return NULL;
}

Customer* removeCustomer(char* id) {
    Customer* head = getAllCustomers();
    Customer* current = head;
    Customer* prev = NULL;
    Customer* removed = NULL;

    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            removed = current;
            if (prev == NULL) {
                head = current->next;
            }
            else {
                prev->next = current->next;
            }
            removed->next = NULL;
            break;
        }
        prev = current;
        current = current->next;
    }

    if (removed == NULL) {
        printf("Customer with ID %s not found.\n", id);
        current = head;
        while (current != NULL) {
            Customer* temp = current;
            current = current->next;
            free(temp->id);
            free(temp->name);
            free(temp->joinDate);
            if (temp->purchases != NULL) {
                for (int i = 0; i < temp->purchaseCount; i++) {
                    free(temp->purchases[i].serialNumber);
                    free(temp->purchases[i].purchaseDate);
                }
                free(temp->purchases);
            }
            free(temp);
        }
        return NULL;
    }

    printf("Customer with ID %s has been removed.\n", id);

    current = head;
    while (current != NULL) {
        Customer* temp = current;
        current = current->next;
        free(temp->id);
        free(temp->name);
        free(temp->joinDate);
        if (temp->purchases != NULL) {
            for (int i = 0; i < temp->purchaseCount; i++) {
                free(temp->purchases[i].serialNumber);
                free(temp->purchases[i].purchaseDate);
            }
            free(temp->purchases);
        }
        free(temp);
    }

    return removed;
}

Customer* updateCustomer(char* id, char* property, void* value) {
    Customer* head = getAllCustomers();
    Customer* current = head;

    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            if (strcmp(property, "id") == 0) {
                char* trimmedID = trimwhitespace((char*)value);
                current->id = (char*)realloc(current->id, strlen(trimmedID) + 1);
                if (current->id == NULL) {
                    printf("Error: Memory allocation failed for id.\n");
                    return NULL;
                }
                strcpy(current->id, trimmedID);
            }
            else if (strcmp(property, "name") == 0) {
                char* trimmedName = trimwhitespace((char*)value);
                current->name = (char*)realloc(current->name, strlen(trimmedName) + 1);
                if (current->name == NULL) {
                    printf("Error: Memory allocation failed for name.\n");
                    return NULL;
                }
                strcpy(current->name, trimmedName);
            }
            else if (strcmp(property, "total_amount_spent") == 0) {
                current->totalAmountSpent = *(double*)value;
            }
            else if (strcmp(property, "items_purchased") == 0) {
                current->itemsPurchased = *(int*)value;
            }
            else if (strcmp(property, "purchases") == 0) {
                int itemFound = 0;
                for (int i = 0; i < current->purchaseCount; i++) {
                    if (strcmp(current->purchases[i].serialNumber, (*(Purchase*)value).serialNumber) == 0) {
                        current->purchases[i].amount += (*(Purchase*)value).amount;
                        strncpy(current->purchases[i].purchaseDate, (*(Purchase*)value).purchaseDate, PURCHASE_DATE_LENGTH);
                        itemFound = 1;
                        break;
                    }
                }
                if (!itemFound) {
                    current->purchases = (Purchase*)realloc(current->purchases,
                        sizeof(Purchase) * (current->purchaseCount + 1));
                    if (current->purchases == NULL) {
                        printf("Error: Memory allocation failed for new purchase.\n");
                        return NULL;
                    }
                    current->purchases[current->purchaseCount].serialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
                    current->purchases[current->purchaseCount].purchaseDate = (char*)malloc(sizeof(char) * PURCHASE_DATE_LENGTH);
                    strcpy(current->purchases[current->purchaseCount].serialNumber, (*(Purchase*)value).serialNumber);
                    strcpy(current->purchases[current->purchaseCount].purchaseDate, (*(Purchase*)value).purchaseDate);
                    current->purchases[current->purchaseCount].amount = (*(Purchase*)value).amount;
                    current->purchaseCount++;
                }
            }
            else {
                printf("Invalid property: %s\n", property);
                return NULL;
            }
            printf("Customer with ID %s has been updated.\n", id);
            return current;
        }
        current = current->next;
    }
    printf("Customer with ID %s not found.\n", id);
    return NULL;
}


void removeCustomerMenu() {
    clrscr();
    printf("Remove Customer Menu:\n");
    char customerID[ID_LENGTH];
    printf("Please enter Customer ID: ");
    scanf("%s", customerID);
    removeCustomer(customerID);
}

void addNewCustomer() {
    char id[ID_LENGTH], name[NAME_LENGTH];
    clearBuffer();
    printf("Add Customer Menu:\n");
    printf("Please enter Customer ID: ");
    scanf("%s", id);
    clearBuffer();
    printf("Please enter Customer Name: ");
    getInputString(name, NAME_LENGTH);
    Customer* customer = createCustomer(id, name);
    addCustomer(customer);
    printCustomers();
    printf("Customer has been added successfully!\n");
}

void checkCustomerPurchases() {
    char id[ID_LENGTH];
    printf("Check Customer Purchases Menu:\n");
    printf("Please enter Customer ID: ");
    scanf("%s", id);
    Customer* customer = findCustomersByProperty("id", id);
    if (customer != NULL)
        printCustomer(id);
}