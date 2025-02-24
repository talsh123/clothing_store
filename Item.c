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
    FILE* file = fopen(fileName, "wb");

    fwrite(item->serialNumber, SERIAL_NUMBER_LENGTH, 1, file);
    fwrite(item->brand, BRAND_LENGTH, 1, file);
    fwrite(item->type, TYPE_LENGTH, 1, file);
    fwrite(&item->price, PRICE_LENGTH, 1, file);
    fwrite(&item->isPopular, IS_POPULAR_LENGTH, 1, file);
    fwrite(item->releaseDate, RELEASE_DATE_LENGTH, 1, file);
    fwrite(&item->stock, STOCK_LENGTH, 1, file);

    fclose(file);
}

Item* readItem(FILE* file) {
    if (file == NULL) {
        return NULL; // File pointer is NULL
    }

    char* temp_serial_number = (char*)malloc(SERIAL_NUMBER_LENGTH * sizeof(char));
    char* temp_brand = (char*)malloc(BRAND_LENGTH * sizeof(char));
    char* temp_type = (char*)malloc(TYPE_LENGTH * sizeof(char));
    double temp_price;
    int temp_isPopular;
    char* temp_releaseDate = (char*)malloc(RELEASE_DATE_LENGTH * sizeof(char));
    int temp_stock;

    // Validate memory allocation for strings
    assert(temp_serial_number);
    assert(temp_brand);
    assert(temp_type);
    assert(temp_releaseDate);

    // Read data from the binary file
    fread(temp_serial_number, sizeof(char), SERIAL_NUMBER_LENGTH, file);
    char* trimmed_serial_number = trimwhitespace(temp_serial_number);
    fread(temp_brand, sizeof(char), BRAND_LENGTH, file);
    char* trimmed_brand = trimwhitespace(temp_brand);
    fread(temp_type, sizeof(char), TYPE_LENGTH, file);
    char* trimmed_type = trimwhitespace(temp_type);
    fread(&temp_price, sizeof(double), 1, file);
    fread(&temp_isPopular, sizeof(int), 1, file);
    fread(temp_releaseDate, sizeof(char), RELEASE_DATE_LENGTH, file);
    char* trimmed_releaseDate = trimwhitespace(temp_releaseDate);
    fread(&temp_stock, sizeof(int), 1, file);

    // Create a new item
    Item* item = createItem(trimmed_serial_number, trimmed_brand, trimmed_type, temp_price, temp_isPopular, trimmed_releaseDate, temp_stock);

    // Free temporary memory
    free(temp_serial_number);
    free(temp_brand);
    free(temp_type);
    free(temp_releaseDate);

    return item;
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
        case 3:
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
            //searchByDate();
            break;
        case 0:
            exit = 1;
            break;
        }
        if (exit == 1)
            break;
    }
}