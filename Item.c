#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Item.h"
#include "Customer.h"
#include "StringOperations.h"
#include <assert.h>

void addItemToList(ItemNode* newNode) {
    if (newNode == NULL) {
        return;
    }
    if (globalItems == NULL) {
        globalItems = newNode;
    }
    else {
        ItemNode* current = globalItems;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

ItemNode* createItem(char* serialNumber, char* brand, char* type, double price, int isPopular, char* releaseDate, int stock) {
    
    Item* newItem = (Item*)safeMalloc(sizeof(Item));

    newItem->serialNumber = (char*)safeMalloc(SERIAL_NUMBER_LENGTH);
    strcpy(newItem->serialNumber, serialNumber);

    newItem->brand = (char*)safeMalloc(BRAND_LENGTH);
    strcpy(newItem->brand, brand);

    newItem->type = (char*)safeMalloc(TYPE_LENGTH);
    strcpy(newItem->type, type);

    newItem->releaseDate = (char*)safeMalloc(RELEASE_DATE_LENGTH);
    strcpy(newItem->releaseDate, releaseDate);

    newItem->stock = stock;
    newItem->price = price;
    newItem->isPopular = isPopular;

    // Create the new linked list node.
    ItemNode* newNode = (ItemNode*)safeMalloc(sizeof(ItemNode));
    newNode->item = newItem;
    newNode->next = NULL;

    // Automatically add the new node to the global items list.
    addItemToList(newNode);

    return newNode;
}

void writeItem(Item* item, FILE* file) {
    if (file == NULL) {
        printf("Error: Invalid file pointer.\n");
        return;
    }

    // Write each field with the exact length
    fwrite(item->serialNumber, sizeof(char), SERIAL_NUMBER_LENGTH, file);
    fwrite(item->brand, sizeof(char), BRAND_LENGTH, file);
    fwrite(item->type, sizeof(char), TYPE_LENGTH, file);
    fwrite(&(item->price), sizeof(double), 1, file);
    fwrite(&(item->isPopular), sizeof(int), 1, file);
    fwrite(item->releaseDate, sizeof(char), RELEASE_DATE_LENGTH, file);
    fwrite(&(item->stock), sizeof(int), 1, file);
}

void saveItemsFromLinkedList(const char* fileName) {

    FILE* file = fopen(fileName, "wb");
    if (!file) {
        printf("Error opening file for clearing.\n");
        return;
    }
    ItemNode* current = globalItems;
    while (current != NULL) {
        writeItem(current->item, file);
        current = current->next;
    }
    fclose(file);
}

Item* readItem(FILE* file) {
    if (file == NULL) {
        return NULL;
    }

    // Check if we are at the end of the file before reading
    int check;
    check = fgetc(file);
    if (check == EOF) {
        return NULL; // EOF reached
    }
    ungetc(check, file); // Put back the character

    // Allocate memory for the item
    Item* item = (Item*)malloc(sizeof(Item));
    if (!item) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // Allocate memory for strings
    item->serialNumber = (char*)malloc(SERIAL_NUMBER_LENGTH * sizeof(char));
    item->brand = (char*)malloc(BRAND_LENGTH * sizeof(char));
    item->type = (char*)malloc(TYPE_LENGTH * sizeof(char));
    item->releaseDate = (char*)malloc(RELEASE_DATE_LENGTH * sizeof(char));

    // Validate memory allocation
    if (!item->serialNumber || !item->brand || !item->type || !item->releaseDate) {
        printf("Memory allocation failed.\n");
        free(item);
        return NULL;
    }

    // Read data from the binary file in the same order as it was written
    if (fread(item->serialNumber, sizeof(char), SERIAL_NUMBER_LENGTH, file) != SERIAL_NUMBER_LENGTH ||
        fread(item->brand, sizeof(char), BRAND_LENGTH, file) != BRAND_LENGTH ||
        fread(item->type, sizeof(char), TYPE_LENGTH, file) != TYPE_LENGTH ||
        fread(&(item->price), sizeof(double), 1, file) != 1 ||
        fread(&(item->isPopular), sizeof(int), 1, file) != 1 ||
        fread(item->releaseDate, sizeof(char), RELEASE_DATE_LENGTH, file) != RELEASE_DATE_LENGTH ||
        fread(&(item->stock), sizeof(int), 1, file) != 1) {

        free(item->serialNumber);
        free(item->brand);
        free(item->type);
        free(item->releaseDate);
        free(item);
        return NULL; // Reached EOF or encountered a read error
    }

    // Ensure null-termination for strings
    item->serialNumber[SERIAL_NUMBER_LENGTH - 1] = '\0';
    item->brand[BRAND_LENGTH - 1] = '\0';
    item->type[TYPE_LENGTH - 1] = '\0';
    item->releaseDate[RELEASE_DATE_LENGTH - 1] = '\0';

    item->serialNumber = trimwhitespace(item->serialNumber);
    item->brand = trimwhitespace(item->brand);
    item->type = trimwhitespace(item->type);
    item->releaseDate = trimwhitespace(item->releaseDate);

    return item;
}

// Updated getAllItems function
Item* getAllItems() {
    FILE* fp = fopen(ITEMS_FILE, "rb");
    if (fp == NULL) {
        printf("Error opening file: %s\n", ITEMS_FILE);
        return NULL;
    }

    Item* tempItem;
    ItemNode* lastNode = NULL;

    // Read all Items and insert them into the linked list
    while ((tempItem = readItem(fp)) != NULL) {
        // Allocate memory for each field of the Item
        tempItem->serialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
        tempItem->brand = (char*)malloc(sizeof(char) * BRAND_LENGTH);
        tempItem->type = (char*)malloc(sizeof(char) * TYPE_LENGTH);
        tempItem->releaseDate = (char*)malloc(sizeof(char) * RELEASE_DATE_LENGTH);

        // Hard copy the Item
        strcpy(tempItem->serialNumber, trimwhitespace(tempItem->serialNumber));
        strcpy(tempItem->brand, trimwhitespace(tempItem->brand));
        strcpy(tempItem->type, trimwhitespace(tempItem->type));
        strcpy(tempItem->releaseDate, trimwhitespace(tempItem->releaseDate));

        // Create a new ItemNode for the linked list
        ItemNode* newNode = createItem(tempItem->serialNumber, tempItem->brand, tempItem->type, tempItem->price, tempItem->isPopular, tempItem->releaseDate, tempItem->stock);

        // If the list is empty, the new node becomes the head
        if (globalItems == NULL) {
            globalItems = newNode;
        }
        else {
            // Otherwise, link the new node at the end of the list
            lastNode->next = newNode;
        }

        // Update the last node pointer to the newly added node
        lastNode = newNode;
    }

    fclose(fp);
    printf("All items have been loaded into the linked list.\n");
    return NULL;
}


void viewItemsMenu() {
    printf("View Items Menu:\n");
    printf("1. Search By Brand and/or Type.\n");
    printf("2. Search By Price and/or Stock.\n");
    printf("3. Search By Equals.\n");
    printf("4. Search By Date.\n");
    printf("0. Exit\n");
}

void printItems() {
    if (globalItems == NULL) {
        printf("No items to display.\n");
        return;
    }

    // Print table header
    printf("+----------------+----------------+----------------+----------+------------+--------------+-------+\n");
    printf("| Serial Number  | Brand          | Type           | Price    | Is Popular | Release Date | Stock |\n");
    printf("+----------------+----------------+----------------+----------+------------+--------------+-------+\n");

    // Iterate through the list and print each item in a formatted row.
    ItemNode* current = globalItems;
    while (current != NULL) {
        printf("| %-14s | %-14s | %-14s | %8.2f | %-10s | %-12s | %5d |\n",
            current->item->serialNumber,
            current->item->brand,
            current->item->type,
            current->item->price,
            current->item->isPopular ? "Yes" : "No",
            current->item->releaseDate,
            current->item->stock);
        current = current->next;
    }
    printf("+----------------+----------------+----------------+----------+------------+--------------+-------+\n");
}

ItemNode* findByBrandType(char* userBrand, char* userType, int filterType) {
    ItemNode* matchingList = NULL;
    ItemNode* tail = NULL;
    ItemNode* current = globalItems;  // globalItems is our global linked list

    while (current != NULL) {
        int matches = 0;
        if (filterType == 1) {
            // Filter by brand only
            if (strcmp(current->item->brand, userBrand) == 0) {
                matches = 1;
            }
        }
        else if (filterType == 2) {
            // Filter by type only
            if (strcmp(current->item->type, userType) == 0) {
                matches = 1;
            }
        }
        else if (filterType == 3) {
            // Filter by both brand and type
            if (strcmp(current->item->brand, userBrand) == 0 &&
                strcmp(current->item->type, userType) == 0) {
                matches = 1;
            }
        }

        if (matches) {
            // Allocate a new node for the matching item (shallow copy)
            ItemNode* newNode = (ItemNode*)safeMalloc(sizeof(ItemNode));
            newNode->item = current->item;
            newNode->next = NULL;

            // Append to the matching list
            if (matchingList == NULL) {
                matchingList = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        current = current->next;
    }

    if (matchingList == NULL) {
        printf("There are no items that matched your search!\n");
    }
    return matchingList;
}

ItemNode* findByPrice(double price, char identifier) {
    // Determine the comparison type based on the identifier.
    int identifierType;
    if (identifier == '>')
        identifierType = 1;
    else if (identifier == '<')
        identifierType = 2;
    else if (identifier == '=')
        identifierType = 3;
    else {
        printf("Invalid identifier '%c'. Please use '>', '<', or '='.\n", identifier);
        return NULL;
    }

    ItemNode* matchingList = NULL;
    ItemNode* tail = NULL;
    ItemNode* current = globalItems;  // Global linked list

    // Iterate through linked list.
    while (current != NULL) {
        int matches = 0;
        if (identifierType == 1) {
            if (current->item->price > price)
                matches = 1;
        }
        else if (identifierType == 2) {
            if (current->item->price < price)
                matches = 1;
        }
        else if (identifierType == 3) {
            if (current->item->price == price)
                matches = 1;
        }

        if (matches) {
            // Allocate a new node (shallow copy: pointer to same Item).
            ItemNode* newNode = (ItemNode*)safeMalloc(sizeof(ItemNode));
            newNode->item = current->item;
            newNode->next = NULL;

            // Append to the matching list.
            if (matchingList == NULL) {
                matchingList = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }

        current = current->next;
    }

    if (matchingList == NULL) {
        printf("There are no items that matched your search!\n");
    }

    return matchingList;
}

ItemNode* findByStock(int stock, char identifier) {
    int identifierType;
    if (identifier == '>')
        identifierType = 1;
    else if (identifier == '<')
        identifierType = 2;
    else if (identifier == '=')
        identifierType = 3;
    else {
        printf("Invalid identifier '%c'. Please use '>', '<', or '='.\n", identifier);
        return NULL;
    }

    ItemNode* matchingList = NULL;
    ItemNode* tail = NULL;
    ItemNode* current = globalItems;  // Global linked list

    while (current != NULL) {
        int matches = 0;
        if (identifierType == 1) {
            if (current->item->stock > stock)
                matches = 1;
        }
        else if (identifierType == 2) {
            if (current->item->stock < stock)
                matches = 1;
        }
        else if (identifierType == 3) {
            if (current->item->stock == stock)
                matches = 1;
        }

        if (matches) {
            // Allocate a new node (shallow copy: pointer to same Item).
            ItemNode* newNode = (ItemNode*)safeMalloc(sizeof(ItemNode));
            newNode->item = current->item;
            newNode->next = NULL;

            // Append to the matching list.
            if (matchingList == NULL) {
                matchingList = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }

        current = current->next;
    }

    if (matchingList == NULL) {
        printf("There are no items that matched your search!\n");
    }

    return matchingList;
}

ItemNode* findByProperty(char* property, void* value) {
    ItemNode* matchingList = NULL;
    ItemNode* tail = NULL;
    ItemNode* current = globalItems;  // Global linked list

    while (current != NULL) {
        int matches = 0;
        if (strcmp(property, "serial_number") == 0) {
            if (strcmp(current->item->serialNumber, (char*)value) == 0)
                matches = 1;
        }
        else if (strcmp(property, "brand") == 0) {
            if (strcmp(current->item->brand, (char*)value) == 0)
                matches = 1;
        }
        else if (strcmp(property, "type") == 0) {
            if (strcmp(current->item->type, (char*)value) == 0)
                matches = 1;
        }
        else if (strcmp(property, "price") == 0) {
            if (current->item->price == *(double*)value)
                matches = 1;
        }
        else if (strcmp(property, "is_popular") == 0) {
            if (current->item->isPopular == *(int*)value)
                matches = 1;
        }
        else if (strcmp(property, "release_date") == 0) {
            if (strcmp(current->item->releaseDate, (char*)value) == 0)
                matches = 1;
        }
        else if (strcmp(property, "stock") == 0) {
            if (current->item->stock == *(int*)value)
                matches = 1;
        }
        else {
            printf("Invalid property: %s\n", property);
            return NULL;
        }

        if (matches) {
            // Allocate a new node (shallow copy: points to the same Item)
            ItemNode* newNode = (ItemNode*)safeMalloc(sizeof(ItemNode));
            newNode->item = current->item;
            newNode->next = NULL;

            // Append the node to the matching list
            if (matchingList == NULL) {
                matchingList = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        current = current->next;
    }

    if (matchingList == NULL) {
        printf("There are no items that matched your search!\n");
    }
    return matchingList;
}

ItemNode* findByDate(char* userDate, char identifier) {
    // Map the identifier to a comparison type:
    // '>' means item date is after userDate, '<' means before, '=' means equal.
    int identifierType;
    if (identifier == '>')
        identifierType = 1;
    else if (identifier == '<')
        identifierType = 2;
    else if (identifier == '=')
        identifierType = 3;
    else {
        printf("Invalid identifier '%c'. Please use '>', '<', or '='.\n", identifier);
        return NULL;
    }

    ItemNode* matchingList = NULL;
    ItemNode* tail = NULL;
    ItemNode* current = globalItems;  // Global linked list of items

    while (current != NULL) {
        // Use compareDates from string operations
        // -1 before, 1 after, 0 equals
        int cmp = compareDates(current->item->releaseDate, userDate);
        int matches = 0;
        if (identifierType == 1 && cmp == 1)
            matches = 1;
        else if (identifierType == 2 && cmp == -1)
            matches = 1;
        else if (identifierType == 3 && cmp == 0)
            matches = 1;

        if (matches) {
            // Allocate a new node (shallow copy: points to the same Item).
            ItemNode* newNode = (ItemNode*)safeMalloc(sizeof(ItemNode));
            newNode->item = current->item;
            newNode->next = NULL;

            // Append newNode to the matching list.
            if (matchingList == NULL) {
                matchingList = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        current = current->next;
    }

    if (matchingList == NULL) {
        printf("There are no items that matched your search!\n");
    }
    return matchingList;
}

ItemNode* findDatesInRange(char* userDate1, char* userDate2) {
    ItemNode* matchingList = NULL;
    ItemNode* tail = NULL;
    ItemNode* current = globalItems;  // Global linked list of items

    while (current != NULL) {
        // isDateBetween returns 1 if current item's releaseDate is between userDate1 and userDate2
        int result = isDateBetween(userDate1, userDate2, current->item->releaseDate);
        if (result == 1) {
            // Allocate a new node (shallow copy: pointer to the same Item)
            ItemNode* newNode = (ItemNode*)safeMalloc(sizeof(ItemNode));
            newNode->item = current->item;
            newNode->next = NULL;

            // Append the new node to the matching list.
            if (matchingList == NULL) {
                matchingList = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        current = current->next;
    }

    if (matchingList == NULL) {
        printf("There are no items that matched your search!\n");
    }

    return matchingList;
}

ItemNode* findItemBySerialNumber(char* serialNumber) {
    ItemNode* current = globalItems;
    while (current != NULL) {
        if (strcmp(current->item->serialNumber, serialNumber) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Item* updateItem(char* userSerialNumber, int property, void* value) {
    // Search for the item in the linked list by its serial number.
    ItemNode* current = globalItems;
    while (current != NULL) {
        if (strcmp(current->item->serialNumber, userSerialNumber) == 0) {
            break;
        }
        current = current->next;
    }

    if (current == NULL) {
        printf("Item with Serial Number %s not found.\n", userSerialNumber);
        return NULL;
    }

    // Update the specified property.
    switch (property) {
    case 1: // Brand
        strncpy(current->item->brand, (char*)value, BRAND_LENGTH - 1);
        current->item->brand[BRAND_LENGTH - 1] = '\0'; // Ensure null-termination
        break;
    case 2: // Type
        strncpy(current->item->type, (char*)value, TYPE_LENGTH - 1);
        current->item->type[TYPE_LENGTH - 1] = '\0';
        break;
    case 3: // Price
        current->item->price = *(double*)value;
        break;
    case 4: // Is Popular
        current->item->isPopular = *(int*)value;
        break;
    case 5: // Release Date
        strncpy(current->item->releaseDate, (char*)value, RELEASE_DATE_LENGTH - 1);
        current->item->releaseDate[RELEASE_DATE_LENGTH - 1] = '\0';
        break;
    case 6: // Stock
        current->item->stock = *(int*)value;
        break;
    default:
        printf("Invalid property.\n");
        return NULL;
    }

    printf("Item with Serial Number %s has been updated.\n", userSerialNumber);

    // Return pointer to the updated item.
    return current->item;
}

void freeItem(Item* item) {
    if (item != NULL) {
        free(item->serialNumber);
        free(item->brand);
        free(item->type);
        free(item->releaseDate);
        free(item);
    }
}

Item* removeItem(char* serialNumber) {
    ItemNode* prev = NULL;
    ItemNode* current = globalItems;

    // Traverse the linked list to find the matching item.
    while (current != NULL) {
        if (strcmp(current->item->serialNumber, serialNumber) == 0) {
            break;
        }
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Item with serial number %s not found.\n", serialNumber);
        return NULL;
    }

    if (prev == NULL) {
        globalItems = current->next;
    }
    else {
        prev->next = current->next;
    }

    Item* removedItem = current->item;
    free(current);

    printf("Item with serial number %s has been removed.\n", serialNumber);
    return removedItem;
}

Item* sellItem(char* itemSerialNumber, char* userCustomerID, int amount) {
    // Check that the sale amount does not exceed 3 items per purchase.
    if (amount > 3) {
        printf("Cannot sell more than 3 items in a single purchase.\n");
        return NULL;
    }

    ItemNode* foundNode = findItemBySerialNumber(itemSerialNumber);
    if (foundNode == NULL) {
        printf("Item not found.\n");
        return NULL;
    }

    Item* item = foundNode->item;

    if (item->stock < amount) {
        printf("Not enough stock available.\n");
        return NULL;
    }

    // Retrieve the customer data.
    Customer* customer = findCustomersByProperty("id", userCustomerID);
    if (customer == NULL) {
        printf("Customer not found.\n");
        return NULL;
    }

    // Update the item's stock.
    item->stock -= amount;

    // Update customer details.
    customer->totalAmountSpent += (item->price * amount);
    customer->itemsPurchased += amount;

    printf("Item sold successfully.\n");
    return item;
}

void sellItemMenu() {
    clrscr();
    printf("Sell Item Menu:\n");

    char* userCustomerID = (char*)safeMalloc(sizeof(char) * ID_LENGTH);
    printf("Please enter Customer ID: ");
    scanf("%s", userCustomerID);

    int totalItemsSold = 0;
    char continueSelling = 'y';

    // Loop while the total items sold is less than 3 and user wishes to continue.
    while (totalItemsSold < 3 && (continueSelling == 'y' || continueSelling == 'Y')) {
        // Get Item Serial Number.
        char* userSerialNumber = (char*)safeMalloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
        printf("Please enter Item Serial Number: ");
        scanf("%s", userSerialNumber);

        // Get the amount of items to sell.
        int itemAmount;
        printf("Please enter the amount you want to sell: ");
        scanf("%d", &itemAmount);

        // Validate the amount.
        if (itemAmount <= 0) {
            printf("Invalid amount. It must be a positive integer.\n");
            free(userSerialNumber);
            continue;
        }

        // Ensure total items sold will not exceed 3.
        if (totalItemsSold + itemAmount > 3) {
            printf("Cannot sell this amount. Total items sold would exceed the limit of 3.\n");
            free(userSerialNumber);
            continue;
        }

        Item* soldItem = sellItem(userSerialNumber, userCustomerID, itemAmount);
        if (soldItem != NULL) {
            printf("Item sold successfully.\n");
            totalItemsSold += itemAmount;
        }
        else {
            printf("Sale failed.\n");
        }

        // Ask if the user wants to sell more items, if under the limit.
        if (totalItemsSold < 3) {
            printf("Would you like to sell more items to this customer? [y/n]: ");
            scanf(" %c", &continueSelling);
        }
        else {
            printf("You have reached the maximum of 3 items for this customer.\n");
        }

        free(userSerialNumber);
    }

    free(userCustomerID);
}

void removeItemMenu() {
    clrscr();
    printf("Remove Item Menu:\n");
    printf("Please enter Item Serial Number: ");

    char* userSerialNumber = (char*)safeMalloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
    scanf("%s", userSerialNumber);

    Item* removedItem = removeItem(userSerialNumber);

    if (removedItem != NULL) {
        freeItem(removedItem);
    }

    free(userSerialNumber);
}

void updateItemMenu() {
    clrscr();
    printf("Update Item Menu:\n");

    // Allocate memory for the serial number.
    char* userSerialNumber = (char*)safeMalloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
    printf("Please enter the item's Serial Number: ");
    scanf("%s", userSerialNumber);

    // Present property options.
    printf("Which property would you like to update:\n");
    printf("1. Update Brand.\n");
    printf("2. Update Type.\n");
    printf("3. Update Price.\n");
    printf("4. Update Is Popular.\n");
    printf("5. Update Release Date.\n");
    printf("6. Update Stock.\n");
    printf("0. Exit\n");
    printf("Please select a property: ");

    int property;
    scanf("%d", &property);

    switch (property) 
    {
        case 1: {
            char* brand = (char*)safeMalloc(sizeof(char) * BRAND_LENGTH);
            printf("Enter new Brand: ");
            scanf("%s", brand);
            updateItem(userSerialNumber, property, brand);
            free(brand);
            break;
        }
        case 2: {
            char* type = (char*)safeMalloc(sizeof(char) * TYPE_LENGTH);
            printf("Enter new Type: ");
            scanf("%s", type);
            updateItem(userSerialNumber, property, type);
            free(type);
            break;
        }
        case 3: {
            double price;
            printf("Enter new Price: ");
            scanf("%lf", &price);
            updateItem(userSerialNumber, property, &price);
            break;
        }
        case 4: {
            int isPopular;
            printf("Enter new Is Popular (1 for Yes, 0 for No): ");
            scanf("%d", &isPopular);
            updateItem(userSerialNumber, property, &isPopular);
            break;
        }
        case 5: {
            char* releaseDate = (char*)safeMalloc(sizeof(char) * RELEASE_DATE_LENGTH);
            printf("Enter new Release Date (DD-MM-YYYY): ");
            scanf("%s", releaseDate);
            updateItem(userSerialNumber, property, releaseDate);
            free(releaseDate);
            break;
        }
        case 6: {
            int stock;
            printf("Enter new Stock: ");
            scanf("%d", &stock);
            updateItem(userSerialNumber, property, &stock);
            break;
        }
        case 0: {
            printf("Exiting update menu.\n");
            break;
        }
        default: 
        {
            printf("Invalid choice.\n");
            break;
        }
    }

    free(userSerialNumber);
}

void searchByBrandOrType() {
    int exit = 0;
    while (!exit) {
        clrscr();
        printf("Search By Brand and/or Type Menu:\n");
        printf("1. Search By Brand\n");
        printf("2. Search By Type\n");
        printf("3. Search By Both\n");
        printf("0. Exit\n");
        printf("Please select an option: ");

        int user_choice;
        scanf("%d", &user_choice);
        clearBuffer();  // Clear leftover characters

        ItemNode* matchingItems = NULL;
        char* brand = NULL, * type = NULL;

        switch (user_choice) {
        case 1:
            brand = (char*)safeMalloc(BRAND_LENGTH);
            printf("Enter Brand: ");
            getInputString(brand, BRAND_LENGTH);
            // Filter by brand only (filterType = 1)
            matchingItems = findByBrandType(brand, NULL, 1);
            free(brand);
            break;
        case 2:
            type = (char*)safeMalloc(TYPE_LENGTH);
            printf("Enter Type: ");
            getInputString(type, TYPE_LENGTH);
            // Filter by type only (filterType = 2)
            matchingItems = findByBrandType(NULL, type, 2);
            free(type);
            break;
        case 3:
            brand = (char*)safeMalloc(BRAND_LENGTH);
            type = (char*)safeMalloc(TYPE_LENGTH);
            printf("Enter Brand: ");
            getInputString(brand, BRAND_LENGTH);
            printf("Enter Type: ");
            getInputString(type, TYPE_LENGTH);
            // Filter by both brand and type (filterType = 3)
            matchingItems = findByBrandType(brand, type, 3);
            free(brand);
            free(type);
            break;
        case 0:
            exit = 1;
            break;
        default:
            printf("Invalid option, please try again.\n");
            break;
        }

        if (matchingItems != NULL) {
            // Print the results in a table.
            printItems(matchingItems);

            // Free the temporary matching list nodes.
            ItemNode* temp;
            while (matchingItems != NULL) {
                temp = matchingItems;
                matchingItems = matchingItems->next;
                free(temp);
            }
        }

        if (!exit) {
            printf("Press Enter to continue...");
            getchar();
        }
    }
}

void searchByPriceorStock() {
    int exit = 0;
    while (!exit) {
        clrscr();
        printf("Search By Price and/or Stock Menu:\n");
        printf("1. Search By Price\n");
        printf("2. Search By Stock\n");
        printf("0. Exit\n");
        printf("Please select an option: ");

        int user_choice;
        scanf("%d", &user_choice);
        clearBuffer(); // Clear any leftover input

        ItemNode* matchingItems = NULL;
        char identifier;

        switch (user_choice) {
        case 1: {
            double userPrice;
            printf("Enter a price: ");
            scanf("%lf", &userPrice);
            clearBuffer();
            printf("Enter an identifier [>, <, =]: ");
            scanf("%c", &identifier);
            clearBuffer();
            matchingItems = findByPrice(userPrice, identifier);
            break;
        }
        case 2: {
            int userStock;
            printf("Enter a stock value: ");
            scanf("%d", &userStock);
            clearBuffer();
            printf("Enter an identifier [>, <, =]: ");
            scanf("%c", &identifier);
            clearBuffer();
            matchingItems = findByStock(userStock, identifier);
            break;
        }
        case 0:
            exit = 1;
            break;
        default:
            printf("Invalid option, please try again.\n");
            break;
        }

        if (matchingItems != NULL) {
            // Print matching items in a table.
            printItems(matchingItems);

            // Free the matching list nodes (shallow copy; items remain in global list).
            ItemNode* temp;
            while (matchingItems != NULL) {
                temp = matchingItems;
                matchingItems = matchingItems->next;
                free(temp);
            }
        }

        if (!exit) {
            printf("Press Enter to continue...");
            getchar();
        }
    }
}

void searchByEquals() {
    int exit = 0;
    while (!exit) {
        clrscr();
        printf("Search a Property Menu:\n");
        printf("1. Search By Serial Number\n");
        printf("2. Search By Brand\n");
        printf("3. Search By Type\n");
        printf("4. Search By Price\n");
        printf("5. Search By Is Popular\n");
        printf("6. Search By Release Date\n");
        printf("7. Search By Stock\n");
        printf("0. Exit\n");
        printf("Please select an option: ");

        int choice;
        scanf("%d", &choice);
        clearBuffer();

        ItemNode* matchingItems = NULL;

        switch (choice) {
        case 1: {
            char* serialNumber = (char*)safeMalloc(SERIAL_NUMBER_LENGTH * sizeof(char));
            printf("Enter Serial Number: ");
            getInputString(serialNumber, SERIAL_NUMBER_LENGTH);
            matchingItems = findByProperty("serial_number", serialNumber);
            free(serialNumber);
            break;
        }
        case 2: {
            char* brand = (char*)safeMalloc(BRAND_LENGTH * sizeof(char));
            printf("Enter Brand: ");
            getInputString(brand, BRAND_LENGTH);
            matchingItems = findByProperty("brand", brand);
            free(brand);
            break;
        }
        case 3: {
            char* type = (char*)safeMalloc(TYPE_LENGTH * sizeof(char));
            printf("Enter Type: ");
            getInputString(type, TYPE_LENGTH);
            matchingItems = findByProperty("type", type);
            free(type);
            break;
        }
        case 4: {
            double price;
            printf("Enter Price: ");
            scanf("%lf", &price);
            clearBuffer();
            matchingItems = findByProperty("price", &price);
            break;
        }
        case 5: {
            int isPopular;
            printf("Enter Is Popular (1 for Yes, 0 for No): ");
            scanf("%d", &isPopular);
            clearBuffer();
            matchingItems = findByProperty("is_popular", &isPopular);
            break;
        }
        case 6: {
            char* releaseDate = (char*)safeMalloc(RELEASE_DATE_LENGTH * sizeof(char));
            printf("Enter Release Date (DD-MM-YYYY): ");
            getInputString(releaseDate, RELEASE_DATE_LENGTH);
            matchingItems = findByProperty("release_date", releaseDate);
            free(releaseDate);
            break;
        }
        case 7: {
            int stock;
            printf("Enter Stock: ");
            scanf("%d", &stock);
            clearBuffer();
            matchingItems = findByProperty("stock", &stock);
            break;
        }
        case 0:
            exit = 1;
            break;
        default:
            printf("Invalid option. Please try again.\n");
            break;
        }

        if (matchingItems != NULL) {
            printItems(matchingItems);

            // Free the temporary matching list nodes.
            ItemNode* temp;
            while (matchingItems != NULL) {
                temp = matchingItems;
                matchingItems = matchingItems->next;
                free(temp);
            }
        }

        if (!exit) {
            printf("Press Enter to continue...");
            getchar();
        }
    }
}

void searchByDate() {
    int exit = 0;
    while (!exit) {
        clrscr();
        printf("Search By Date Menu:\n");
        printf("1. Search by a single date with an identifier\n");
        printf("2. Search by a date range\n");
        printf("0. Exit\n");
        printf("Please select an option: ");

        int choice;
        scanf("%d", &choice);
        clearBuffer();

        ItemNode* matchingItems = NULL;

        switch (choice) {
        case 1: {
            char* userDate = (char*)safeMalloc(sizeof(char) * RELEASE_DATE_LENGTH);
            printf("Enter Release Date (DD-MM-YYYY): ");
            getInputString(userDate, RELEASE_DATE_LENGTH);

            char identifier;
            printf("Enter an identifier [>, <, =]: ");
            scanf("%c", &identifier);
            clearBuffer();

            matchingItems = findByDate(userDate, identifier);
            free(userDate);
            break;
        }
        case 2: {
            char* date1 = (char*)safeMalloc(sizeof(char) * RELEASE_DATE_LENGTH);
            char* date2 = (char*)safeMalloc(sizeof(char) * RELEASE_DATE_LENGTH);
            printf("Enter the start date (DD-MM-YYYY): ");
            getInputString(date1, RELEASE_DATE_LENGTH);
            printf("Enter the end date (DD-MM-YYYY): ");
            getInputString(date2, RELEASE_DATE_LENGTH);

            matchingItems = findDatesInRange(date1, date2);
            free(date1);
            free(date2);
            break;
        }
        case 0:
            exit = 1;
            break;
        default:
            printf("Invalid option. Please try again.\n");
            break;
        }

        if (matchingItems != NULL) {
            printItems(matchingItems);

            // Free the temporary matching list nodes (shallow copies)
            ItemNode* temp;
            while (matchingItems != NULL) {
                temp = matchingItems;
                matchingItems = matchingItems->next;
                free(temp);
            }
        }

        if (!exit) {
            printf("Press Enter to continue...");
            getchar();
        }
    }
}

void viewItems() {
    int exit = 0;
    while (!exit) {
        clrscr();
        viewItemsMenu();
        int user_choice;
        printf("Please select an option: ");
        scanf("%d", &user_choice);
        clearBuffer();

        switch (user_choice) {
        case 1:
            // Search by Brand or Type
            searchByBrandOrType();
            break;
        case 2:
            // Search by Price or Stock
            searchByPriceorStock();
            break;
        case 3:
            // Search by Equals (generic search by property)
            searchByEquals();
            break;
        case 4:
            // Search by Date (single date or range)
            searchByDate();
            break;
        case 0:
            exit = 1;
            break;
        default:
            printf("Invalid option. Please try again.\n");
            break;
        }
        if (!exit) {
            printf("Press Enter to continue...");
            getchar();
        }
    }
}

void addNewItem() {
    clrscr();
    printf("Add Item Menu:\n");

    // Allocate buffers for string inputs.
    char* serial_number = (char*)safeMalloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
    char* brand = (char*)safeMalloc(sizeof(char) * BRAND_LENGTH);
    char* type = (char*)safeMalloc(sizeof(char) * TYPE_LENGTH);
    char* releaseDate = (char*)safeMalloc(sizeof(char) * RELEASE_DATE_LENGTH);

    double price;
    int isPopular;
    int stock;

    // Get Serial Number
    printf("Please enter Serial Number: ");
    scanf("%s", serial_number);
    clearBuffer();

    // Get Brand
    printf("Please enter Brand: ");
    getInputString(brand, BRAND_LENGTH);

    // Get Type
    printf("Please enter Type: ");
    getInputString(type, TYPE_LENGTH);

    // Get Price
    printf("Please enter Price: ");
    scanf("%lf", &price);
    clearBuffer();

    // Get Is Popular flag
    printf("Is the Item Popular? [1 - Yes / 0 - No]: ");
    scanf("%d", &isPopular);
    clearBuffer();

    // Get Release Date
    printf("Please enter Release Date (DD-MM-YYYY): ");
    scanf("%s", releaseDate);
    clearBuffer();

    // Get Stock
    printf("Please enter Stock: ");
    scanf("%d", &stock);
    clearBuffer();

    // Create the new item (this automatically adds it to the global linked list)
    ItemNode* newNode = createItem(serial_number, brand, type, price, isPopular, releaseDate, stock);

    printf("\nNew item added:\n");
    printf("+----------------+----------------+----------------+----------+------------+--------------+-------+\n");
    printf("| Serial Number  | Brand          | Type           | Price    | Is Popular | Release Date | Stock |\n");
    printf("+----------------+----------------+----------------+----------+------------+--------------+-------+\n");
    printf("| %-14s | %-14s | %-14s | %8.2f | %-10s | %-12s | %5d |\n",
        newNode->item->serialNumber,
        newNode->item->brand,
        newNode->item->type,
        newNode->item->price,
        newNode->item->isPopular ? "Yes" : "No",
        newNode->item->releaseDate,
        newNode->item->stock);
    printf("+----------------+----------------+----------------+----------+------------+--------------+-------+\n");
    free(serial_number);
    free(brand);
    free(type);
    free(releaseDate);
}

ItemNode* loadItemsLinkedList(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        return NULL;
    }

    ItemNode* head = NULL;
    ItemNode* tail = NULL;
    while (1) {
        Item* tempItem = readItem(fp);
        if (tempItem == NULL) {
            break;
        }
        // Create a new node for the item using safeMalloc
        ItemNode* newNode = (ItemNode*)safeMalloc(sizeof(ItemNode));
        newNode->item = tempItem;
        newNode->next = NULL;

        // Append to the linked list
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    fclose(fp);
    return head;
}
