#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Item.h"
#include "FileOperations.h"
#include "StringOperations.h"
#include <assert.h>

#define ITEMS_FILE "items.bin"

// Item FILE STRUCTURE
#define SERIAL_NUMBER_LENGTH 13
#define BRAND_LENGTH 31
#define TYPE_LENGTH 12
#define PRICE_LENGTH 8
#define IS_POPULAR_LENGTH 4
#define RELEASE_DATE_LENGTH 11
#define STOCK_LENGTH 4


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

int compareitems(const Item* item1, const Item* item2) {
    // Check if usernames are the same
    if (strcmp(item1->serialNumber, item2->serialNumber) != 0 ||
        strcmp(item1->brand, item2->brand) != 0 ||
        strcmp(item1->type, item2->type) != 0 ||
        item1->price != item2->price || 
        strcmp(item1->isPopular, item2->isPopular) != 0 ||
        strcmp(item1->releaseDate, item2->releaseDate) != 0 ||
        item1->stock != item2->stock) {
        return 0;
    }
    return 1;
}

void writeItem(Item* item, const char* fileName) {
    FILE* file = fopen(fileName, "ab");
    if (file == NULL) {
        printf("Error opening file for appending.\n");
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

    fclose(file);
}

void writeItems(Item* items, int itemCount, const char* fileName) {
    FILE* file = fopen(fileName, "wb");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    for (int i = 0; i < itemCount; i++) {
        // Write each field with the exact length
        fwrite(items[i].serialNumber, sizeof(char), SERIAL_NUMBER_LENGTH, file);
        fwrite(items[i].brand, sizeof(char), BRAND_LENGTH, file);
        fwrite(items[i].type, sizeof(char), TYPE_LENGTH, file);
        fwrite(&(items[i].price), sizeof(double), 1, file);
        fwrite(&(items[i].isPopular), sizeof(int), 1, file);
        fwrite(items[i].releaseDate, sizeof(char), RELEASE_DATE_LENGTH, file);
        fwrite(&(items[i].stock), sizeof(int), 1, file);
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
    printItems(items, *itemCount);
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

Item* updateItem(char* userSerialNumber, int property, void* value) {
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

    // Update the specified property
    switch (property) {
    case 1: // Brand
        strncpy(items[foundIndex].brand, (char*)value, BRAND_LENGTH - 1);
        items[foundIndex].brand[BRAND_LENGTH - 1] = '\0'; // Ensure null-termination
        break;
    case 2: // Type
        strncpy(items[foundIndex].type, (char*)value, TYPE_LENGTH - 1);
        items[foundIndex].type[TYPE_LENGTH - 1] = '\0'; // Ensure null-termination
        break;
    case 3: // Price
        items[foundIndex].price = *(double*)value;
        break;
    case 4: // Is Popular
        items[foundIndex].isPopular = *(int*)value;
        break;
    case 5: // Release Date
        strncpy(items[foundIndex].releaseDate, (char*)value, RELEASE_DATE_LENGTH - 1);
        items[foundIndex].releaseDate[RELEASE_DATE_LENGTH - 1] = '\0'; // Ensure null-termination
        break;
    case 6: // Stock
        items[foundIndex].stock = *(int*)value;
        break;
    default:
        printf("Invalid property.\n");
        return NULL;
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
        strcpy(removedItem->brand, items[foundIndex].brand);
        strcpy(removedItem->type, items[foundIndex].type);
        strcpy(removedItem->releaseDate, items[foundIndex].releaseDate);
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
    int property;
    scanf("%d", &property);

    printf("Please enter a new value: ");
    switch (property) {
    case 1: {
        char* brand = (char*)malloc(sizeof(char) * BRAND_LENGTH);
        scanf("%s", brand);
        updateItem(userSerialNumber, property, brand);
        free(brand);
        break;
    }
    case 2: {
        char* type = (char*)malloc(sizeof(char) * TYPE_LENGTH);
        scanf("%s", type);
        updateItem(userSerialNumber, property, type);
        free(type);
        break;
    }
    case 3: {
        double price;
        scanf("%lf", &price);
        updateItem(userSerialNumber, property, &price);
        break;
    }
    case 4: {
        int isPopular;
        scanf("%d", &isPopular);
        updateItem(userSerialNumber, property, &isPopular);
        break;
    }
    case 5: {
        char* releaseDate = (char*)malloc(sizeof(char) * RELEASE_DATE_LENGTH);
        scanf("%s", releaseDate);
        updateItem(userSerialNumber, property, releaseDate);
        free(releaseDate);
        break;
    }
    case 6: {
        int stock;
        scanf("%d", &stock);
        updateItem(userSerialNumber, property, &stock);
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
    int exit = 0;
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
    printf("Please enter Brand: ");
    scanf("%s", brand);
    printf("Please enter Type: ");
    scanf("%s", type);
    printf("Please enter price: ");
    scanf("%lf", price);
    printf("Is the Item Popular? [1 - Yes/0 - No]: ");
    scanf("%d", isPopular);
    printf("Please enter Release Date: ");
    scanf("%s", releaseDate);
    printf("Please enter Stock: ");
    scanf("%d", stock);
    Item* item = createItem(serial_number, brand, type, *price, *isPopular, releaseDate, *stock);
    printItems(item, 1);
    writeItem(item, ITEMS_FILE);
    printf("Item has been written to file successfully!\n");
}