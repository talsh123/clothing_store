#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Item.h"
#include "Customer.h"
#include "StringOperations.h"
#include <assert.h>

Item* createItem(char* serialNumber, char* brand, char* type, double price, int isPopular, char* releaseDate, int stock) {
    Item* item = (Item*)malloc(sizeof(Item));

    item->serialNumber = (char*)malloc(SERIAL_NUMBER_LENGTH);
    strcpy(item->serialNumber, serialNumber);

    item->brand = (char*)malloc(BRAND_LENGTH);
    strcpy(item->brand, brand);

    item->type = (char*)malloc(TYPE_LENGTH);
    strcpy(item->type, type);

    item->price = price;

    item->isPopular = isPopular;

    item->releaseDate = (char*)malloc(RELEASE_DATE_LENGTH);
    strcpy(item->releaseDate, releaseDate);

    item->stock = stock;

    return item;
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

void writeItems(Item* items, int itemCount, const char* fileName) {
    // Step 1: Clear the file first
    FILE* file = fopen(fileName, "wb");
    if (file == NULL) {
        printf("Error opening file for clearing.\n");
        return;
    }
    fclose(file);

    // Step 2: Re-open in append mode and write each item
    file = fopen(fileName, "ab");
    if (file == NULL) {
        printf("Error opening file for appending.\n");
        return;
    }

    for (int i = 0; i < itemCount; i++) {
        writeItem(&items[i], file);
    }

    fclose(file);
}

Item* readItem(FILE* file) {
    if (file == NULL) {
        return NULL; // File pointer is NULL
    }

    // Allocate memory for the item
    Item* item = (Item*)malloc(sizeof(Item));
    if (item == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // Allocate memory for strings
    item->serialNumber = (char*)malloc(SERIAL_NUMBER_LENGTH * sizeof(char));
    item->brand = (char*)malloc(BRAND_LENGTH * sizeof(char));
    item->type = (char*)malloc(TYPE_LENGTH * sizeof(char));
    item->releaseDate = (char*)malloc(RELEASE_DATE_LENGTH * sizeof(char));

    // Validate memory allocation
    assert(item->serialNumber);
    assert(item->brand);
    assert(item->type);
    assert(item->releaseDate);

    // Read data from the binary file in the same order as it was written
    fread(item->serialNumber, sizeof(char), SERIAL_NUMBER_LENGTH, file);
    fread(item->brand, sizeof(char), BRAND_LENGTH, file);
    fread(item->type, sizeof(char), TYPE_LENGTH, file);
    fread(&(item->price), sizeof(double), 1, file);
    fread(&(item->isPopular), sizeof(int), 1, file);
    fread(item->releaseDate, sizeof(char), RELEASE_DATE_LENGTH, file);
    fread(&(item->stock), sizeof(int), 1, file);

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

Item* getAllItems(int* itemCount) {
    FILE* fp = fopen(ITEMS_FILE, "rb");

    Item* items = NULL;
    *itemCount = 0;
    Item* tempItem;

    // Read all Items
    while ((tempItem = readItem(fp)) != NULL) {
        // Check for end of file after attempting to read
        if (feof(fp)) {
            break;
        }

        // Dynamically allocate memory
        items = realloc(items, sizeof(Item) * (*itemCount + 1));

        // Hard copy the Item
        items[*itemCount].serialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
        items[*itemCount].brand = (char*)malloc(sizeof(char) * BRAND_LENGTH);
        items[*itemCount].type = (char*)malloc(sizeof(char) * TYPE_LENGTH);
        items[*itemCount].releaseDate = (char*)malloc(sizeof(char) * RELEASE_DATE_LENGTH);
        
        strcpy(items[*itemCount].serialNumber, (*tempItem).serialNumber);
        items[*itemCount].serialNumber = trimwhitespace(items[*itemCount].serialNumber);
        
        strcpy(items[*itemCount].brand, (*tempItem).brand);
        items[*itemCount].brand = trimwhitespace(items[*itemCount].brand);
        
        strcpy(items[*itemCount].type, (*tempItem).type);
        items[*itemCount].type = trimwhitespace(items[*itemCount].type);
        
        items[*itemCount].price = (*tempItem).price;
        
        items[*itemCount].isPopular = (*tempItem).isPopular;
        
        strcpy(items[*itemCount].releaseDate, (*tempItem).releaseDate);
        items[*itemCount].releaseDate = trimwhitespace(items[*itemCount].releaseDate);
        
        items[*itemCount].stock = (*tempItem).stock;

        // Increment the itemCount
        (*itemCount)++;

        if (fp == NULL) {
            printf("Error opening file.\n");
            return NULL;
        }
    }

    fclose(fp);
    return items;
}

void viewItemsMenu() {
    printf("View Items Menu:\n");
    printf("1. Search By Brand and/or Type.\n");
    printf("2. Search By Price and/or Stock.\n");
    printf("3. Search By Equals.\n");
    printf("4. Search By Date.\n");
    printf("0. Exit\n");
}

// Function to print all items in one line each
void printItems(Item* items, int itemCount) {
    if (items == NULL || itemCount <= 0) {
        printf("No items to display.\n");
        return;
    }

    printf("\n--- Items List ---\n");
    for (int i = 0; i < itemCount; i++) {
        printf("%s, %s, %s, %.2f, %s, %s, %d\n",
            items[i].serialNumber,
            items[i].brand,
            items[i].type,
            items[i].price,
            items[i].isPopular ? "Yes" : "No",
            items[i].releaseDate,
            items[i].stock
        );
    }
    printf("--------------------\n");
}

Item* findByBrandType(char* userBrand, char* userType, int filterType) {
    // Open the binary file for reading
    FILE* file = fopen(ITEMS_FILE, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", ITEMS_FILE);
        return NULL;
    }

    // Dynamic array to store matching items
    Item* matchingItems = NULL;
    int count = 0;

    // Read items one by one
    Item* item;
    while ((item = readItem(file)) != NULL) {
        // Check for end of file after attempting to read
        if (feof(file)) {
            break;
        }

        int matches = 0;

        // Apply filter type logic
        if (filterType == 1) {
            // Filter by brand
            if (strcmp(item->brand, userBrand) == 0) {
                matches = 1;
            }
        }
        else if (filterType == 2) {
            // Filter by type
            if (strcmp(item->type, userType) == 0) {
                matches = 1;
            }
        }
        else if (filterType == 3) {
            // Filter by both brand and type
            if (strcmp(item->brand, userBrand) == 0 && strcmp(item->type, userType) == 0) {
                matches = 1;
            }
        }

        // If item matches, add it to the dynamic array
        if (matches) {
            // Resize array to hold another item
            Item* tempArray = realloc(matchingItems, sizeof(Item) * (count + 1));
            if (tempArray == NULL) {
                printf("Error: Memory allocation failed.\n");
                free(matchingItems); // Free previously allocated memory
                fclose(file);
                return NULL;
            }
            matchingItems = tempArray;

            // Copy the item into the array
            matchingItems[count] = *item;
            count++;
        }

        // Free the memory for the current item
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
        free(matchingItems);
    }
    // Print all the matching Items
    else {
        // Also print all the matching Items
        printItems(matchingItems, count);
        free(matchingItems);
    }
    return matchingItems;
}

Item* findByPrice(double price, char identifier) {
    // Open the binary file for reading
    FILE* file = fopen(ITEMS_FILE, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", ITEMS_FILE);
        return NULL;
    }

    // Dynamic array to store matching items
    Item* matchingItems = NULL;
    int count = 0;

    int identifierType;

    // Map identifier
    if (identifier == '>')
        identifierType = 1;
    else if (identifier == '<')
        identifierType = 2;
    else
        identifierType = 3;

    // Read items one by one
    Item* item;
    while ((item = readItem(file)) != NULL) {
        // Check for end of file after attempting to read
        if (feof(file)) {
            break;
        }

        int matches = 0;

        // Apply filter type logic
        if (identifierType == 1) {
            if (item->price > price) {
                matches = 1;
            }
        }
        else if (identifierType == 2) {
            if (item->price < price) {
                matches = 1;
            }
        }
        else if (identifierType == 3) {
            if (item->price == price) {
                matches = 1;
            }
        }

        // If item matches, add it to the dynamic array
        if (matches) {
            // Resize array to hold another item
            Item* tempArray = realloc(matchingItems, sizeof(Item) * (count + 1));
            if (tempArray == NULL) {
                printf("Error: Memory allocation failed.\n");
                free(matchingItems); // Free previously allocated memory
                fclose(file);
                return NULL;
            }
            matchingItems = tempArray;

            // Copy the item into the array
            matchingItems[count] = *item;
            count++;
        }

        // Free the memory for the current item
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
    }
    // Print all the matching Items
    else {
        // Also print all the matching Items
        printItems(matchingItems, count);
    }
    return matchingItems;
}

Item* findByStock(int stock, char identifier) {
    // Open the binary file for reading
    FILE* file = fopen(ITEMS_FILE, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", ITEMS_FILE);
        return NULL;
    }

    // Dynamic array to store matching items
    Item* matchingItems = NULL;
    int count = 0;

    int identifierType;

    // Map identifier
    if (identifier == '>')
        identifierType = 1;
    else if (identifier == '<')
        identifierType = 2;
    else if (identifier == '=')
        identifierType = 3;
    else
        return matchingItems;

    // Read items one by one
    Item* item;
    while ((item = readItem(file)) != NULL) {
        // Check for end of file after attempting to read
        if (feof(file)) {
            break;
        }

        int matches = 0;

        // Apply filter type logic
        if (identifierType == 1) {
            if (item->stock > stock) {
                matches = 1;
            }
        }
        else if (identifierType == 2) {
            if (item->stock < stock) {
                matches = 1;
            }
        }
        else if (identifierType == 3) {
            if (item->stock == stock) {
                matches = 1;
            }
        }

        // If item matches, add it to the dynamic array
        if (matches) {
            // Resize array to hold another item
            Item* tempArray = realloc(matchingItems, sizeof(Item) * (count + 1));
            if (tempArray == NULL) {
                printf("Error: Memory allocation failed.\n");
                free(matchingItems); // Free previously allocated memory
                fclose(file);
                return NULL;
            }
            matchingItems = tempArray;

            // Copy the item into the array
            matchingItems[count] = *item;
            count++;
        }

        // Free the memory for the current item
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
    }
    // Print all the matching Items
    else {
        // Also print all the matching Items
        printItems(matchingItems, count);
    }
    return matchingItems;
}

Item* findByProperty(char* property, void* value) {
    FILE* file = fopen(ITEMS_FILE, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", ITEMS_FILE);
        return NULL;
    }

    // Dynamic array to store matching items
    Item* matchingItems = NULL;

    // Read items one by one
    Item* item;
    int count = 0;
    while ((item = readItem(file)) != NULL) {
        // Check for end of file after attempting to read
        if (feof(file)) {
            break;
        }

        int matches = 0;

        // Check property and compare value
        if (strcmp(property, "serial_number") == 0) {
            if (strcmp(item->serialNumber, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "brand") == 0) {
            if (strcmp(item->brand, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "type") == 0) {
            if (strcmp(item->type, (char*)value) == 0) {
                matches = 1;
            }
        }
        else if (strcmp(property, "price") == 0) {
            if (item->price == *(double*)value) {
                matches = 1;
            }
        }
        else if (strcmp(property, "stock") == 0) {
            if (item->stock == *(int*)value) {
                matches = 1;
            }
        }
        else if (strcmp(property, "is_popular") == 0) {
            if (item->isPopular == *(int*)value) {
                matches = 1;
            }
        }
        else if (strcmp(property, "release_date") == 0) {
            if (strcmp(item->releaseDate, (char*)value) == 0) {
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
            Item* tempArray = realloc(matchingItems, sizeof(Item) * count);
            if (tempArray == NULL) {
                printf("Error: Memory allocation failed.\n");
                fclose(file);
                return NULL;
            }
            matchingItems = tempArray;
            matchingItems[count - 1] = *item;
        }

        // Free the memory for the current item
        free(item);
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
    }
    // Print all the matching Items
    else {
        // Also print all the matching Items
        printItems(matchingItems, count);
    }
    return matchingItems;
}

Item* findByDate(char* userDate, char identifier) {
    // Open the binary file for reading
    FILE* file = fopen(ITEMS_FILE, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", ITEMS_FILE);
        return NULL;
    }

    // Dynamic array to store matching items
    Item* matchingItems = NULL;
    int count = 0;

    int identifierType;

    // Map identifier
    if (identifier == '>')
        identifierType = 1;
    else if (identifier == '<')
        identifierType = -1;
    else if (identifier == '=')
        identifierType = 0;
    else
        return matchingItems;

    // Read items one by one
    Item* item;
    while ((item = readItem(file)) != NULL) {
        // Check for end of file after attempting to read
        if (feof(file)) {
            break;
        }

        int matches = 0;

        // Compare the dates
        // -1 if the first date is before the second.
        // 1 if the first date is after the second.
        // 0 if both dates are equal.
        int result = compareDates(item->releaseDate, userDate);

        if(identifierType == result)
            matches = 1;

        // If item matches, add it to the dynamic array
        if (matches) {
            // Resize array to hold another item
            Item* tempArray = realloc(matchingItems, sizeof(Item) * (count + 1));
            if (tempArray == NULL) {
                printf("Error: Memory allocation failed.\n");
                free(matchingItems); // Free previously allocated memory
                fclose(file);
                return NULL;
            }
            matchingItems = tempArray;

            // Copy the item into the array
            matchingItems[count] = *item;
            count++;
        }
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
    }
    // Print all the matching Items
    else {
        // Also print all the matching Items
        printItems(matchingItems, count);
    }
    return matchingItems;
}

Item* findDatesInRange(char* userDate1, char* userDate2) {
    // Open the binary file for reading
    FILE* file = fopen(ITEMS_FILE, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", ITEMS_FILE);
        return NULL;
    }

    // Dynamic array to store matching items
    Item* matchingItems = NULL;
    int count = 0;

    // Read items one by one
    Item* item;
    while ((item = readItem(file)) != NULL) {
        // Check for end of file after attempting to read
        if (feof(file)) {
            break;
        }

        int matches = 0;

        // Compare the dates
        // 1 if the date between the first 2 dates
        // 0 Otherwise
        int result = isDateBetween(userDate1, userDate2, item->releaseDate);

        if (result == 1)
            matches = 1;

        // If item matches, add it to the dynamic array
        if (matches) {
            // Resize array to hold another item
            Item* tempArray = realloc(matchingItems, sizeof(Item) * (count + 1));
            if (tempArray == NULL) {
                printf("Error: Memory allocation failed.\n");
                free(matchingItems); // Free previously allocated memory
                fclose(file);
                return NULL;
            }
            matchingItems = tempArray;

            // Copy the item into the array
            matchingItems[count] = *item;
            count++;
        }
    }

    fclose(file);

    // If no items matched, free memory and return NULL
    if (count == 0) {
        printf("There are no items that matched your search!\n");
    }
    // Print all the matching Items
    else {
        // Also print all the matching Items
        printItems(matchingItems, count);
    }
    return matchingItems;
}

Item* updateItem(char* userSerialNumber, char* property, void* value) {
    int itemCount = 0;
    Item* items = getAllItems(&itemCount);

    if (items == NULL || itemCount == 0) {
        printf("No items found.\n");
        return NULL;
    }

    int foundIndex = -1;
    // Find the item with the given Serial Number
    for (int i = 0; i < itemCount; i++) {
        if (strcmp(items[i].serialNumber, userSerialNumber) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("Item with Serial Number %s not found.\n", userSerialNumber);
        return NULL;
    }

    if (strcmp(property, "brand") == 0) {
        strncpy(items[foundIndex].brand, (char*)value, BRAND_LENGTH - 1);
        items[foundIndex].brand[BRAND_LENGTH - 1] = '\0'; // Ensure null-termination
    }
    if (strcmp(property, "type") == 0) {
        strncpy(items[foundIndex].type, (char*)value, TYPE_LENGTH - 1);
        items[foundIndex].type[TYPE_LENGTH - 1] = '\0'; // Ensure null-termination
    }
    if (strcmp(property, "price") == 0) {
        items[foundIndex].price = *(double*)value;
    }
    if (strcmp(property, "is_popular") == 0) {
        items[foundIndex].isPopular = *(int*)value;
    }
    if (strcmp(property, "release_date") == 0) {
        strncpy(items[foundIndex].releaseDate, (char*)value, RELEASE_DATE_LENGTH - 1);
        items[foundIndex].releaseDate[RELEASE_DATE_LENGTH - 1] = '\0'; // Ensure null-termination
    }
    if (strcmp(property, "stock") == 0) {
        items[foundIndex].stock = *(int*)value;
    }

    // Save the updated items back to the file
    writeItems(items, itemCount, ITEMS_FILE);

    printf("Item with Serial Number %s has been updated.\n", userSerialNumber);

    return &(items[foundIndex]);
}


Item* removeItem(char* serialNumber) {
    int itemCount = 0;
    Item* items = getAllItems(&itemCount);
    if (items == NULL || itemCount == 0) {
        printf("No items to remove.\n");
        return NULL;
    }

    Item* removedItem = NULL;
    int foundIndex = -1;

    // Search for the item with the given serialNumber
    for (int i = 0; i < itemCount; i++) {
        if (strcmp(items[i].serialNumber, serialNumber) == 0) {
            foundIndex = i;
            break;
        }
    }

    // If item is found
    if (foundIndex != -1) {
        // Allocate memory for the removed item and copy its data
        removedItem = (Item*)malloc(sizeof(Item));
        removedItem->serialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
        removedItem->brand = (char*)malloc(sizeof(char) * BRAND_LENGTH);
        removedItem->type = (char*)malloc(sizeof(char) * TYPE_LENGTH);
        removedItem->releaseDate = (char*)malloc(sizeof(char) * RELEASE_DATE_LENGTH);

        // Copy item data to the removed item
        strcpy(removedItem->serialNumber, items[foundIndex].serialNumber);
        removedItem->serialNumber = trimwhitespace(removedItem->serialNumber);
        strcpy(removedItem->brand, items[foundIndex].brand);
        removedItem->brand = trimwhitespace(removedItem->brand);
        strcpy(removedItem->type, items[foundIndex].type);
        removedItem->type = trimwhitespace(removedItem->type);
        strcpy(removedItem->releaseDate, items[foundIndex].releaseDate);
        removedItem->releaseDate = trimwhitespace(removedItem->releaseDate);
        removedItem->price = items[foundIndex].price;
        removedItem->isPopular = items[foundIndex].isPopular;
        removedItem->stock = items[foundIndex].stock;

        // Shift items to remove the found item
        for (int i = foundIndex; i < itemCount - 1; i++) {
            items[i] = items[i + 1];
        }

        // Reallocate memory to shrink the items array
        items = realloc(items, sizeof(Item) * (itemCount - 1));
        itemCount--;

        // Update the binary file after removal
        writeItems(items, itemCount, ITEMS_FILE);

        printf("Item with serial number %s has been removed.\n", serialNumber);
    }
    else {
        printf("Item with serial number %s not found.\n", serialNumber);
    }

    printItems(items, itemCount);

    // Free the dynamically allocated memory for items array
    for (int i = 0; i < itemCount; i++) {
        free(items[i].serialNumber);
        free(items[i].type);
        free(items[i].brand);
        free(items[i].releaseDate);
    }
    free(items);

    return removedItem;
}

Item* sellItem(char* itemSerialNumber, char* userCustomerID, int amount) {
    Item* item = findByProperty("serial_number", itemSerialNumber);

    if (amount > 3) {
        printf("Cannot sell more than 3 items in a single purchase.\n");
        return NULL;
    }
    if (item == NULL) {
        printf("Item not found.\n");
        return NULL;
    }
    if (item->stock <= 0) {
        printf("Item is out of stock.\n");
        return NULL;
    }
    if (item->stock < amount) {
        printf("There are not enough items in stock to complete this purchase.\n");
        return NULL;
    }

    Customer* customer = findCustomersByProperty("id", userCustomerID);
    if (customer == NULL) {
        printf("Customer not found.\n");
        return NULL;
    }

    // Check if the item was already purchased by the customer
    int itemFound = 0;
    for (int i = 0; i < customer->purchaseCount; i++) {
        if (strcmp(customer->purchases[i].serialNumber, itemSerialNumber) == 0) {
            // Item found, update the amount
            customer->purchases[i].amount += amount;
            itemFound = 1;
            break;
        }
    }

    // If item was not found, add a new purchase entry
    if (!itemFound) {
        // Reallocate memory for a new purchase entry
        customer->purchases = (Purchase*)realloc(customer->purchases, sizeof(Purchase) * (customer->purchaseCount + 1));
        if (customer->purchases == NULL) {
            printf("Error: Memory allocation failed for new purchase.\n");
            return NULL;
        }

        // Allocate memory for the new serial number and purchase date
        customer->purchases[customer->purchaseCount].serialNumber = (char*)malloc(SERIAL_NUMBER_LENGTH);
        customer->purchases[customer->purchaseCount].purchaseDate = (char*)malloc(PURCHASE_DATE_LENGTH);

        if (customer->purchases[customer->purchaseCount].serialNumber == NULL ||
            customer->purchases[customer->purchaseCount].purchaseDate == NULL) {
            printf("Error: Memory allocation failed for purchase details.\n");
            return NULL;
        }

        // Copy serial number
        strncpy(customer->purchases[customer->purchaseCount].serialNumber, itemSerialNumber, SERIAL_NUMBER_LENGTH - 1);
        customer->purchases[customer->purchaseCount].serialNumber = trimwhitespace(customer->purchases[customer->purchaseCount].serialNumber);

        // Set the amount
        customer->purchases[customer->purchaseCount].amount = amount;

        // Set the purchase date to today
        strncpy(customer->purchases[customer->purchaseCount].purchaseDate, getCurrentDate(), PURCHASE_DATE_LENGTH);

        // Increment the purchase count
        customer->purchaseCount++;
    }

    // Update stock and customer details
    item->stock -= amount;
    customer->totalAmountSpent += (item->price) * amount;
    customer->itemsPurchased += amount;

    // Save updates to file
    updateItem(itemSerialNumber, "stock", &(item->stock));
    updateCustomer(userCustomerID, "total_amount_spent", &(customer->totalAmountSpent));
    updateCustomer(userCustomerID, "items_purchased", &(customer->itemsPurchased));
    //updateCustomer(userCustomerID, "purchase_count", &(customer->purchaseCount));
    // NO NEED TO UPDATE purchase_count, it is being updated when updating purchases
    updateCustomer(userCustomerID, "purchases", customer->purchases);

    // TODO: Log the transaction

    printf("Item sold successfully.\n");
    return item;
}


void sellItemMenu() {
    clrscr();
    printf("Sell Item Menu:\n");

    clearBuffer();

    // Get Customer ID
    printf("Please enter Customer ID: ");
    char* userCustomerID = (char*)malloc(sizeof(char) * ID_LENGTH);
    getInputString(userCustomerID, ID_LENGTH);

    int totalItemsSold = 0;
    char continueSelling = 'y';

    while (totalItemsSold < 3 && (continueSelling == 'y' || continueSelling == 'Y')) {
        // Get Item Serial Number
        printf("Please enter Item Serial Number: ");
        char* userSerialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
        scanf("%s", userSerialNumber);

        // Get Item Amount
        int itemAmount;
        printf("Please enter the amount you want to sell: ");
        scanf("%d", &itemAmount);

        // Validate amount
        if (itemAmount <= 0) {
            printf("Invalid amount. It must be a positive integer.\n");
            free(userSerialNumber);
            continue;
        }

        // Check if total will exceed 3 items
        if (totalItemsSold + itemAmount > 3) {
            printf("Cannot sell this amount. Total items sold would exceed the limit of 3.\n");
            free(userSerialNumber);
            continue;
        }

        // Call sellItem() with the required parameters
        Item* soldItem = sellItem(userSerialNumber, userCustomerID, itemAmount);
        if (soldItem != NULL) {
            printf("Item sold successfully.\n");
            totalItemsSold += itemAmount;
        }

        // Ask if the user wants to sell more items
        if (totalItemsSold < 3) {
            printf("Would you like to sell more items to this customer? [y/n]: ");
            scanf(" %c", &continueSelling);  // Space before %c to ignore newline
        }
        else {
            printf("You have reached the maximum of 3 items for this customer.\n");
        }

        // Free allocated memory
        free(userSerialNumber);
    }

    // Free allocated memory for Customer ID
    free(userCustomerID);
}

void removeItemMenu() {
    clrscr();
    printf("Remove Item Menu:\n");
    printf("Please enter Item Serial Number: ");
    char* userSerialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
    scanf("%s", userSerialNumber);
    removeItem(userSerialNumber);
}

void updateItemMenu() {
    clrscr();
    char* userSerialNumber = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
    printf("Update Item Menu:\n");
    printf("Please enter the item's Serial Number: ");
    scanf("%s", userSerialNumber);
    printf("Which property would you like to update:\n");
    printf("1. Update Brand.\n");
    printf("2. Update Type.\n");
    printf("3. Update Price.\n");
    printf("4. Update Is Popular.\n");
    printf("5. Update Release Date.\n");
    printf("6. Update Stock.\n");
    printf("0. Exit\n");
    printf("Please select a property: ");
    int user_choice;
    scanf("%d", &user_choice);

    printf("Please enter a new value: ");
    switch (user_choice) {
    case 1: {
        char* brand = (char*)malloc(sizeof(char) * BRAND_LENGTH);
        scanf("%s", brand);
        updateItem(userSerialNumber, "brand", brand);
        free(brand);
        break;
    }
    case 2: {
        char* type = (char*)malloc(sizeof(char) * TYPE_LENGTH);
        scanf("%s", type);
        updateItem(userSerialNumber, "type", type);
        free(type);
        break;
    }
    case 3: {
        double price;
        scanf("%lf", &price);
        updateItem(userSerialNumber, "price", &price);
        break;
    }
    case 4: {
        int isPopular;
        scanf("%d", &isPopular);
        updateItem(userSerialNumber, "is_popular", &isPopular);
        break;
    }
    case 5: {
        char* releaseDate = (char*)malloc(sizeof(char) * RELEASE_DATE_LENGTH);
        scanf("%s", releaseDate);
        updateItem(userSerialNumber, "release_date", releaseDate);
        free(releaseDate);
        break;
    }
    case 6: {
        int stock;
        scanf("%d", &stock);
        updateItem(userSerialNumber, "stock", &stock);
        break;
    }
    default:
        printf("Invalid choice.\n");
    }
    free(userSerialNumber);
}


void searchByBrandOrType() {
    int exit = 0;
    clrscr();
    printf("Search By Brand and/or Type Menu:\n");
    printf("1. Search By Brand.\n");
    printf("2. Search By Type\n");
    printf("3. Search By Both\n");
    printf("0. Exit\n");
    while (1) {
        int user_choice;
        printf("Please select: ");
        // Get User Brand and Type
        char* userBrand = NULL;
        char* userType = NULL;
        clearBuffer();
        scanf("%d", &user_choice);
        Item* matchingItems = NULL;
        switch (user_choice) {
        case 1:
            userBrand = (char*)malloc(sizeof(char) * BRAND_LENGTH);
            printf("Please enter Brand: ");
            clearBuffer();
            scanf("%30s", userBrand);
            matchingItems = findByBrandType(userBrand, userType, 1);
            break;
        case 2:
            userType = (char*)malloc(sizeof(char) * TYPE_LENGTH);
            printf("Please enter Type: ");
            clearBuffer();
            scanf("%11s", userType);
            matchingItems = findByBrandType(userBrand, userType, 2);
            break;
        case 3:
            userBrand = (char*)malloc(sizeof(char) * BRAND_LENGTH);
            userType = (char*)malloc(sizeof(char) * TYPE_LENGTH);
            printf("Please enter Brand: ");
            scanf("%30s", userBrand);
            printf("Please enter Type: ");
            scanf("%11s", userType);
            matchingItems = findByBrandType(userBrand, userType, 3);
            break;
        case 0:
            exit = 1;
            break;
        default:
            clrscr();
            printf("No choice was detected, please try again!\n");
            printf("How would you like to filter?\n");
            printf("1. Search By Brand.\n");
            printf("2. Search By Type\n");
            printf("3. Search By Both\n");
            printf("0. Exit\n");
        }
        if (exit == 1)
            break;
    }
}

void searchByPriceorStock() {
    int exit = 0;
    clrscr();
    printf("Search By Price and/or Stock Menu:\n");
    printf("1. Search By Price.\n");
    printf("2. Search By Stock\n");
    printf("0. Exit\n");
    while (1) {
        int user_choice;
        printf("Please select: ");
        // Get User Price and Stock
        double userPrice = 0;
        int userStock = 0;
        char identifier;
        clearBuffer();
        scanf("%d", &user_choice);
        Item* matchingItems = NULL;
        switch (user_choice) {
        case 1:
            printf("Please enter a price: ");
            clearBuffer();
            scanf("%lf", &userPrice);
            printf("Please enter an identifier[>, <, =]: ");
            clearBuffer();
            scanf("%c", &identifier);
            matchingItems = findByPrice(userPrice, identifier);
            break;
        case 2:
            printf("Please enter a stock: ");
            clearBuffer();
            scanf("%d", &userStock);
            printf("Please enter an identifier[>, <, =]: ");
            clearBuffer();
            scanf("%c", &identifier);
            matchingItems = findByStock(userStock, identifier);
            break;
        case 0:
            exit = 1;
            break;
        default:
            clrscr();
            printf("No choice was detected, please try again!\n");
            printf("Search By Price and/or Stock Menu:\n");
            printf("1. Search By Price.\n");
            printf("2. Search By Stock\n");
            printf("0. Exit\n");
        }
        if (exit == 1)
            break;
    }
}

void searchByEquals() {
    int exit = 0;
    clrscr();
    printf("Search a property:\n");
    printf("1. Search By Serial Number.\n");
    printf("2. Search By Brand.\n");
    printf("3. Search By Type.\n");
    printf("4. Search By Price.\n");
    printf("5. Search By Is Popular.\n");
    printf("6. Search By Release Date.\n");
    printf("7. Search By Stock.\n");
    printf("0. Exit\n");
    while (1) {
        int user_choice;
        printf("Please select: ");
        clearBuffer();
        scanf("%d", &user_choice);
        clearBuffer(); // Clear buffer after number input

        Item* matchingItems = NULL;
        void* property = NULL;

        switch (user_choice) {
        case 1: // Serial Number
        {
            char* serialNumber = (char*)malloc(SERIAL_NUMBER_LENGTH * sizeof(char));
            printf("Enter Serial Number: ");
            scanf("%s", serialNumber);
            property = serialNumber;
            matchingItems = findByProperty("serial_number", property);
            free(serialNumber); // Free memory after use
            break;
        }
        case 2: // Brand
        {
            char* brand = (char*)malloc(BRAND_LENGTH * sizeof(char));
            printf("Enter Brand: ");
            scanf("%s", brand);
            property = brand;
            matchingItems = findByProperty("brand", property);
            free(brand);
            break;
        }
        case 3: // Type
        {
            char* type = (char*)malloc(TYPE_LENGTH * sizeof(char));
            printf("Enter Type: ");
            scanf("%s", type);
            property = type;
            matchingItems = findByProperty("type", property);
            free(type);
            break;
        }
        case 4: // Price
        {
            double price;
            printf("Enter Price: ");
            scanf("%lf", &price);
            property = &price;
            matchingItems = findByProperty("price", property);
            break;
        }
        case 5: // Is Popular
        {
            int isPopular;
            printf("Enter Is Popular (0 or 1): ");
            scanf("%d", &isPopular);
            property = &isPopular;
            matchingItems = findByProperty("is_popular", property);
            break;
        }
        case 6: // Release Date
        {
            char* releaseDate = (char*)malloc(RELEASE_DATE_LENGTH * sizeof(char));
            printf("Enter Release Date (DD-MM-YYYY): ");
            scanf("%s", releaseDate);
            property = releaseDate;
            matchingItems = findByProperty("release_date", property);
            free(releaseDate);
            break;
        }
        case 7: // Stock
        {
            int stock;
            printf("Enter Stock: ");
            scanf("%d", &stock);
            property = &stock;
            matchingItems = findByProperty("stock", property);
            break;
        }
        case 0:
            exit = 1;
            break;
        default:
            clrscr();
            printf("Invalid choice, please try again!\n");
        }

        if (matchingItems) {
            // TODO: Print matching items or process them
        }

        if (exit == 1)
            break;
    }
}

void searchByDate() {
    clrscr();
    int exit = 0;
    int user_choice;
    printf("Search By Date Menu\n");
    printf("1. Filter Release Date with identifier.\n");
    printf("2. Find Release Date between Dates.\n");
    printf("0. Exit\n");
    void* property;
    while (1) {
        printf("Please select: ");
        clearBuffer();
        scanf("%d", &user_choice);
        switch (user_choice) {
        case 1: {
            char identifier;
            char* releaseDate = (char*)malloc(RELEASE_DATE_LENGTH * sizeof(char));
            printf("Enter Release Date (DD-MM-YYYY): ");
            scanf("%s", releaseDate);
            printf("Please enter an identifier[>, <, =]: ");
            scanf("%c", &identifier);
            findByDate(releaseDate, identifier);
            break;
        }
        case 2: {
            char* releaseDate1 = (char*)malloc(RELEASE_DATE_LENGTH * sizeof(char));
            char* releaseDate2 = (char*)malloc(RELEASE_DATE_LENGTH * sizeof(char));
            printf("Enter 1st Release Date (DD-MM-YYYY): ");
            scanf("%s", releaseDate1);
            printf("Enter 2nd Release Date (DD-MM-YYYY): ");
            scanf("%s", releaseDate2);
            findDatesInRange(releaseDate1, releaseDate2);
            break;
        }
        case 0:
            exit = 1;
            break;
        }
        if (exit == 1)
            break;
    }
}

void viewItems() {
    while (1) {
        int exit = 0;
        clrscr();
        viewItemsMenu();
        int user_choice;
        clearBuffer();
        printf("Please select: ");
        scanf("%d", &user_choice);
        switch (user_choice) {
        case 1:
            searchByBrandOrType();
            break;
        case 2:
            searchByPriceorStock();
            break;
        case 3:
            searchByEquals();
            break;
        case 4:
            searchByDate();
            break;
        case 0:
            exit = 1;
            break;
        }
        if (exit == 1)
            break;
    }
}

void addNewItem() {
    clrscr();
    char* serial_number = (char*)malloc(sizeof(char) * SERIAL_NUMBER_LENGTH);
    char* brand = (char*)malloc(sizeof(char) * BRAND_LENGTH);
    char* type = (char*)malloc(sizeof(char) * TYPE_LENGTH);
    double* price = malloc(sizeof(double));
    int* isPopular = (int*)malloc(sizeof(int));
    char* releaseDate = (char*)malloc(sizeof(char) * RELEASE_DATE_LENGTH);
    int* stock = (int*)malloc(sizeof(int));
    clearBuffer();
    printf("Add Item Menu:\n");
    printf("Please enter Serial Number: ");
    scanf("%s", serial_number);
    getInputString(serial_number, SERIAL_NUMBER_LENGTH);
    printf("Please enter Brand: ");
    scanf("%s", brand);
    getInputString(serial_number, BRAND_LENGTH);
    printf("Please enter Type: ");
    scanf("%s", type);
    getInputString(serial_number, TYPE_LENGTH);
    printf("Please enter price: ");
    scanf("%lf", price);
    printf("Is the Item Popular? [1 - Yes/0 - No]: ");
    scanf("%d", isPopular);
    printf("Please enter Release Date: ");
    scanf("%s", releaseDate);
    getInputString(releaseDate, RELEASE_DATE_LENGTH);
    printf("Please enter Stock: ");
    scanf("%d", stock);
    Item* item = createItem(serial_number, brand, type, *price, *isPopular, releaseDate, *stock);
    writeItem(item, ITEMS_FILE);
    printItems(item, 1);
    printf("Item has been written to file successfully!\n");
}