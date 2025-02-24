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

void writeitem(Item* item, const char* fileName) {
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

Item* readitem(FILE* file) {
    if (file == NULL) {
        return NULL; // File pointer is NULL
    }

    // Allocate memory for string fields
    char* temp_serial_number = (char*)malloc(SERIAL_NUMBER_LENGTH * sizeof(char));
    char* temp_brand = (char*)malloc(BRAND_LENGTH * sizeof(char));
    char* temp_type = (char*)malloc(TYPE_LENGTH * sizeof(char));
    char* temp_releaseDate = (char*)malloc(RELEASE_DATE_LENGTH * sizeof(char));

    // Validate memory allocation for strings
    assert(temp_serial_number);
    assert(temp_brand);
    assert(temp_type);
    assert(temp_releaseDate);

    // Declare variables for primitive fields
    double temp_price;
    int temp_isPopular;
    int temp_stock;

    // Read data from the binary file
    fread(temp_serial_number, sizeof(char), SERIAL_NUMBER_LENGTH - 1, file);
    temp_serial_number[SERIAL_NUMBER_LENGTH - 1] = '\0'; // Null-terminate the string

    fread(temp_brand, sizeof(char), BRAND_LENGTH - 1, file);
    temp_brand[BRAND_LENGTH - 1] = '\0'; // Null-terminate the string

    fread(temp_type, sizeof(char), TYPE_LENGTH - 1, file);
    temp_type[TYPE_LENGTH - 1] = '\0'; // Null-terminate the string

    fread(&temp_price, sizeof(double), 1, file);
    fread(&temp_isPopular, sizeof(int), 1, file);
    fread(temp_releaseDate, sizeof(char), RELEASE_DATE_LENGTH - 1, file);
    temp_releaseDate[RELEASE_DATE_LENGTH - 1] = '\0'; // Null-terminate the string

    fread(&temp_stock, sizeof(int), 1, file);

    // Create a new item
    Item* item = createItem(temp_serial_number, temp_brand, temp_type, temp_price, temp_isPopular, temp_releaseDate, temp_stock);

    // Free temporary memory
    free(temp_serial_number);
    free(temp_brand);
    free(temp_type);
    free(temp_releaseDate);

    return item;
}

void viewItemsMenu() {
    printf("1. Search By Brand and/or Type.\n");
    printf("2. Search By Price and/or Stock.\n");
    printf("3. Search By Equals.\n");
    printf("4. Search By Date.\n");
    printf("5. Exit\n");
}

void viewItems() {
    viewItemsMenu();
    while (1) {
        int user_choice;
        clearBuffer();
        printf("How would you want to view items? ");
        scanf("%d", &user_choice);
        switch (user_choice) {
        case 1:
            printf("Please enter text: ");

            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        default:
            clrscr();
            printf("No choice was detected, please try again!\n");
            viewItemsMenu();
        }
    }
}