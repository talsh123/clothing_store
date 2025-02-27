#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Customer.h"
#include "Item.h"
#include "StringOperations.h"
#include <assert.h>
#include <time.h>

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
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(customer->joinDate, "%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    // Total Amount Spent
    customer->totalAmountSpent = 0.0;

    // Items Purchased
    customer->itemsPurchased = 0;

    // Purchase Count
    customer->purchaseCount = 0;
    
    // Purchases
    customer->purchases = NULL;

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
    for (int i = 0; i < customer->purchaseCount; i++) {
        fprintf(file, "%-*s ", SERIAL_NUMBER_LENGTH - 1, customer->purchases[i].serialNumber);
        fprintf(file, "%d ", customer->purchases[i].amount);
        fprintf(file, "%-*s ", PURCHASE_DATE_LENGTH - 1, customer->purchases[i].purchaseDate);
    }

    // End line for this customer
    fprintf(file, "\n");

    fclose(file);
}

void writeCustomers(Customer* customers, int customerCount, const char* fileName) {
    // Open the file in write mode to clear its contents
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    fclose(file);

    // Iterate through the customer list and write each one
    for (int i = 0; i < customerCount; i++) {
        writeCustomer(&customers[i], fileName);
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
void printCustomers(Customer* customers, int customerCount) {
    if (customers == NULL || customerCount <= 0) {
        printf("No customers to display.\n");
        return;
    }

    printf("\n--- Customers List ---\n");
    for (int i = 0; i < customerCount; i++) {
        // Print basic customer details
        printf("%s, %s, %s, %.2lf, %d, %d",
            customers[i].id,
            customers[i].name,
            customers[i].joinDate,
            customers[i].totalAmountSpent,
            customers[i].itemsPurchased,
            customers[i].purchaseCount
        );

        // Print purchase history if available
        if (customers[i].purchaseCount > 0 && customers[i].purchases != NULL) {
            printf(" | Purchases: ");
            for (int j = 0; j < customers[i].purchaseCount; j++) {
                printf("{Serial: %s, Amount: %d, Date: %s}",
                    customers[i].purchases[j].serialNumber,
                    customers[i].purchases[j].amount,
                    customers[i].purchases[j].purchaseDate
                );

                // Separate multiple purchases with a comma
                if (j < customers[i].purchaseCount - 1) {
                    printf(", ");
                }
            }
        }
        else {
            printf(" | No Purchases");
        }

        printf("\n");
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

    // Read all Customers
    while ((tempCustomer = readCustomer(fp)) != NULL) {
        // Dynamically allocate memory for the new customer
        customers = realloc(customers, sizeof(Customer) * (*customerCount + 1));
        if (customers == NULL) {
            printf("Error: Memory allocation failed.\n");
            fclose(fp);
            return NULL;
        }

        // Initialize current customer pointer
        Customer* currentCustomer = &customers[*customerCount];

        // Deep Copy Customer Basic Details
        currentCustomer->id = (char*)malloc(sizeof(char) * ID_LENGTH);
        currentCustomer->name = (char*)malloc(sizeof(char) * NAME_LENGTH);
        currentCustomer->joinDate = (char*)malloc(sizeof(char) * JOIN_DATE_LENGTH);

        if (currentCustomer->id == NULL || currentCustomer->name == NULL || currentCustomer->joinDate == NULL) {
            printf("Error: Memory allocation failed for strings.\n");
            fclose(fp);
            return NULL;
        }

        strcpy(currentCustomer->id, tempCustomer->id);
        currentCustomer->id = trimwhitespace(currentCustomer->id);
        strcpy(currentCustomer->name, tempCustomer->name);
        currentCustomer->name = trimwhitespace(currentCustomer->name);
        strcpy(currentCustomer->joinDate, tempCustomer->joinDate);
        currentCustomer->joinDate = trimwhitespace(currentCustomer->joinDate);

        currentCustomer->totalAmountSpent = tempCustomer->totalAmountSpent;
        currentCustomer->itemsPurchased = tempCustomer->itemsPurchased;
        currentCustomer->purchaseCount = tempCustomer->purchaseCount;

        // Deep Copy Purchase History
        if (tempCustomer->purchaseCount > 0 && tempCustomer->purchases != NULL) {
            currentCustomer->purchases = (Purchase*)malloc(sizeof(Purchase) * tempCustomer->purchaseCount);
            if (currentCustomer->purchases == NULL) {
                printf("Error: Memory allocation failed for purchases.\n");
                fclose(fp);
                return NULL;
            }

            for (int i = 0; i < tempCustomer->purchaseCount; i++) {
                // Allocate and copy serialNumber
                currentCustomer->purchases[i].serialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
                if (currentCustomer->purchases[i].serialNumber == NULL) {
                    printf("Error: Memory allocation failed for serialNumber.\n");
                    fclose(fp);
                    return NULL;
                }
                strcpy(currentCustomer->purchases[i].serialNumber, tempCustomer->purchases[i].serialNumber);
                currentCustomer->purchases[i].serialNumber = trimwhitespace(currentCustomer->purchases[i].serialNumber);

                // Copy amount
                currentCustomer->purchases[i].amount = tempCustomer->purchases[i].amount;

                // Allocate and copy purchaseDate
                currentCustomer->purchases[i].purchaseDate = (char*)malloc(sizeof(char) * PURCHASE_DATE_LENGTH);
                if (currentCustomer->purchases[i].purchaseDate == NULL) {
                    printf("Error: Memory allocation failed for purchaseDate.\n");
                    fclose(fp);
                    return NULL;
                }
                strcpy(currentCustomer->purchases[i].purchaseDate, tempCustomer->purchases[i].purchaseDate);
                currentCustomer->purchases[i].purchaseDate = trimwhitespace(currentCustomer->purchases[i].purchaseDate);
            }
        }
        else {
            currentCustomer->purchaseCount = 0;
            currentCustomer->purchases = NULL;
        }

        // Increment the customer count
        (*customerCount)++;

        // Free temporary customer
        free(tempCustomer->id);
        free(tempCustomer->name);
        free(tempCustomer->joinDate);

        if (tempCustomer->purchases != NULL) {
            for (int i = 0; i < tempCustomer->purchaseCount; i++) {
                free(tempCustomer->purchases[i].serialNumber);
                free(tempCustomer->purchases[i].purchaseDate);
            }
            free(tempCustomer->purchases);
        }
        free(tempCustomer);

        // Check for end of file after attempting to read
        if (feof(fp)) {
            break;
        }
    }

    fclose(fp);

    return customers;
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
    FILE* file = fopen(CUSTOMERS_FILE, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", CUSTOMERS_FILE);
        return NULL;
    }

    // Dynamic array to store matching customers
    Customer* matchingCustomers = NULL;
    int count = 0;
    Customer* customer;

    // Read customers one by one
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
        else if (strcmp(property, "purchase_count") == 0) {
            if (customer->purchaseCount == *(int*)value) {
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

            // Allocate and Copy Basic Details
            matchingCustomers[count - 1].id = strdup(customer->id);
            matchingCustomers[count - 1].id = trimwhitespace(matchingCustomers[count - 1].id);
            matchingCustomers[count - 1].name = strdup(customer->name);
            matchingCustomers[count - 1].name = trimwhitespace(matchingCustomers[count - 1].name);
            matchingCustomers[count - 1].joinDate = strdup(customer->joinDate);
            matchingCustomers[count - 1].joinDate = trimwhitespace(matchingCustomers[count - 1].joinDate);
            matchingCustomers[count - 1].totalAmountSpent = customer->totalAmountSpent;
            matchingCustomers[count - 1].itemsPurchased = customer->itemsPurchased;
            matchingCustomers[count - 1].purchaseCount = customer->purchaseCount;

            // Check for memory allocation failure in basic fields
            if (matchingCustomers[count - 1].id == NULL ||
                matchingCustomers[count - 1].name == NULL ||
                matchingCustomers[count - 1].joinDate == NULL) {
                printf("Error: Memory allocation failed for strings.\n");
                fclose(file);
                return NULL;
            }

            // Deep Copy Purchase History
            if (customer->purchaseCount > 0 && customer->purchases != NULL) {
                matchingCustomers[count - 1].purchases = (Purchase*)malloc(sizeof(Purchase) * customer->purchaseCount);
                if (matchingCustomers[count - 1].purchases == NULL) {
                    printf("Error: Memory allocation failed for purchases.\n");
                    fclose(file);
                    return NULL;
                }

                for (int i = 0; i < customer->purchaseCount; i++) {
                    // Allocate and copy serialNumber
                    matchingCustomers[count - 1].purchases[i].serialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
                    if (matchingCustomers[count - 1].purchases[i].serialNumber == NULL) {
                        printf("Error: Memory allocation failed for serialNumber.\n");
                        fclose(file);
                        return NULL;
                    }
                    strcpy(matchingCustomers[count - 1].purchases[i].serialNumber, customer->purchases[i].serialNumber);
                    matchingCustomers[count - 1].purchases[i].serialNumber = trimwhitespace(matchingCustomers[count - 1].purchases[i].serialNumber);

                    // Copy amount
                    matchingCustomers[count - 1].purchases[i].amount = customer->purchases[i].amount;

                    // Allocate and copy purchaseDate
                    matchingCustomers[count - 1].purchases[i].purchaseDate = (char*)malloc(sizeof(char) * PURCHASE_DATE_LENGTH);
                    if (matchingCustomers[count - 1].purchases[i].purchaseDate == NULL) {
                        printf("Error: Memory allocation failed for purchaseDate.\n");
                        fclose(file);
                        return NULL;
                    }
                    strcpy(matchingCustomers[count - 1].purchases[i].purchaseDate, customer->purchases[i].purchaseDate);
                    matchingCustomers[count - 1].purchases[i].purchaseDate = trimwhitespace(matchingCustomers[count - 1].purchases[i].purchaseDate);
                }
            }
            else {
                matchingCustomers[count - 1].purchaseCount = 0;
                matchingCustomers[count - 1].purchases = NULL;
            }
        }

        // Free temporary customer
        free(customer->id);
        free(customer->name);
        free(customer->joinDate);

        if (customer->purchases != NULL) {
            for (int i = 0; i < customer->purchaseCount; i++) {
                free(customer->purchases[i].serialNumber);
                free(customer->purchases[i].purchaseDate);
            }
            free(customer->purchases);
        }
        free(customer);
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
        free(matchingCustomers);
        return NULL;
    }

    return matchingCustomers;
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
        if (removedCustomer == NULL) {
            printf("Error: Memory allocation failed for removed customer.\n");
            return NULL;
        }

        // Copy basic details
        removedCustomer->id = strdup(customers[foundIndex].id);
        removedCustomer->name = strdup(customers[foundIndex].name);
        removedCustomer->joinDate = strdup(customers[foundIndex].joinDate);
        removedCustomer->totalAmountSpent = customers[foundIndex].totalAmountSpent;
        removedCustomer->itemsPurchased = customers[foundIndex].itemsPurchased;
        removedCustomer->purchaseCount = customers[foundIndex].purchaseCount;

        // Check for memory allocation failure in basic fields
        if (removedCustomer->id == NULL ||
            removedCustomer->name == NULL ||
            removedCustomer->joinDate == NULL) {
            printf("Error: Memory allocation failed for strings.\n");
            free(removedCustomer);
            return NULL;
        }

        // Deep Copy Purchase History
        if (customers[foundIndex].purchaseCount > 0 && customers[foundIndex].purchases != NULL) {
            removedCustomer->purchases = (Purchase*)malloc(sizeof(Purchase) * customers[foundIndex].purchaseCount);
            if (removedCustomer->purchases == NULL) {
                printf("Error: Memory allocation failed for purchases.\n");
                free(removedCustomer->id);
                free(removedCustomer->name);
                free(removedCustomer->joinDate);
                free(removedCustomer);
                return NULL;
            }

            for (int i = 0; i < customers[foundIndex].purchaseCount; i++) {
                // Allocate and copy serialNumber
                removedCustomer->purchases[i].serialNumber = strdup(customers[foundIndex].purchases[i].serialNumber);
                removedCustomer->purchases[i].purchaseDate = strdup(customers[foundIndex].purchases[i].purchaseDate);
                removedCustomer->purchases[i].amount = customers[foundIndex].purchases[i].amount;

                // Check for memory allocation failure
                if (removedCustomer->purchases[i].serialNumber == NULL ||
                    removedCustomer->purchases[i].purchaseDate == NULL) {
                    printf("Error: Memory allocation failed for purchase details.\n");
                    for (int j = 0; j <= i; j++) {
                        free(removedCustomer->purchases[j].serialNumber);
                        free(removedCustomer->purchases[j].purchaseDate);
                    }
                    free(removedCustomer->purchases);
                    free(removedCustomer->id);
                    free(removedCustomer->name);
                    free(removedCustomer->joinDate);
                    free(removedCustomer);
                    return NULL;
                }
            }
        }
        else {
            removedCustomer->purchaseCount = 0;
            removedCustomer->purchases = NULL;
        }

        // Shift customers to remove the found item
        for (int i = foundIndex; i < customerCount - 1; i++) {
            customers[i] = customers[i + 1];
        }

        // Reallocate memory to shrink the customers array
        customers = realloc(customers, sizeof(Customer) * (customerCount - 1));
        if (customers == NULL && customerCount > 1) {
            printf("Error: Memory reallocation failed.\n");
            return NULL;
        }
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

        if (customers[i].purchases != NULL) {
            for (int j = 0; j < customers[i].purchaseCount; j++) {
                free(customers[i].purchases[j].serialNumber);
                free(customers[i].purchases[j].purchaseDate);
            }
            free(customers[i].purchases);
        }
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
        free(customers[foundIndex].id);
        customers[foundIndex].id = (char*)malloc(strlen((char*)value) + 1);
        if (customers[foundIndex].id == NULL) {
            printf("Error: Memory allocation failed for id.\n");
            return NULL;
        }
        strcpy(customers[foundIndex].id, (char*)value);
        customers[foundIndex].id = trimwhitespace(customers[foundIndex].id);
        break;
    case 2: // name
        free(customers[foundIndex].name);
        customers[foundIndex].name = (char*)malloc(strlen((char*)value) + 1);
        if (customers[foundIndex].name == NULL) {
            printf("Error: Memory allocation failed for name.\n");
            return NULL;
        }
        strcpy(customers[foundIndex].name, (char*)value);
        customers[foundIndex].name = trimwhitespace(customers[foundIndex].name);
        break;
    default:
        printf("Invalid property.\n");
        return NULL;
    }

    // Save the updated customers back to the file
    writeCustomers(customers, customerCount, CUSTOMERS_FILE);

    printf("Customer with ID %s has been updated.\n", id);

    // Free the dynamically allocated memory for customers array
    for (int i = 0; i < customerCount; i++) {
        free(customers[i].id);
        free(customers[i].name);
        free(customers[i].joinDate);

        if (customers[i].purchases != NULL) {
            for (int j = 0; j < customers[i].purchaseCount; j++) {
                free(customers[i].purchases[j].serialNumber);
                free(customers[i].purchases[j].purchaseDate);
            }
            free(customers[i].purchases);
        }
    }
    free(customers);

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
    clearBuffer();
    printf("Add Customer Menu:\n");
    printf("Please enter Customer ID: ");
    scanf("%s", id);
    clearBuffer();
    printf("Please enter Customer Name: ");
    fgets(name, NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0;
    Customer* customer = createCustomer(id, name);
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
    printCustomers(customer, 1);
}