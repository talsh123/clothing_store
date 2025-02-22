#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Product.h"
#include "FileOperations.h"
#include "StringOperations.h"
#include <assert.h>

#define PRODUCTS_FILE "products.bin"

// Product FILE STRUCTURE
#define SERIAL_NUMBER_LENGTH 13
#define BRAND_LENGTH 31
#define TYPE_LENGTH 12
#define PRICE_LENGTH 8
#define IS_POPULAR_LENGTH 4
#define RELEASE_DATE_LENGTH 11
#define STOCK_LENGTH 4


Product* createProduct(char* serialNumber, char* brand, char* type, double price, int isPopular, char* releaseDate, int stock) {
    Product* product = (Product*)malloc(sizeof(Product));

    product->serialNumber = (char*)malloc(SERIAL_NUMBER_LENGTH);
    strcpy(product->serialNumber, serialNumber);

    product->brand = (char*)malloc(BRAND_LENGTH);
    strcpy(product->brand, brand);

    product->type = (char*)malloc(TYPE_LENGTH);
    strcpy(product->type, type);

    product->price = price;

    product->isPopular = isPopular;

    product->releaseDate = (char*)malloc(RELEASE_DATE_LENGTH);
    strcpy(product->releaseDate, releaseDate);

    product->stock = stock;

    return product;
}

int compareProducts(const Product* product1, const Product* product2) {
    // Check if usernames are the same
    if (strcmp(product1->serialNumber, product2->serialNumber) != 0 ||
        strcmp(product1->brand, product2->brand) != 0 ||
        strcmp(product1->type, product2->type) != 0 ||
        product1->price != product2->price || 
        strcmp(product1->isPopular, product2->isPopular) != 0 ||
        strcmp(product1->releaseDate, product2->releaseDate) != 0 ||
        product1->stock != product2->stock) {
        return 0;
    }
    return 1;
}

void writeProduct(Product* product, const char* fileName) {
    FILE* file = fopen(fileName, "wb");

    fwrite(product->serialNumber, SERIAL_NUMBER_LENGTH, 1, file);
    fwrite(product->brand, BRAND_LENGTH, 1, file);
    fwrite(product->type, TYPE_LENGTH, 1, file);
    fwrite(&product->price, PRICE_LENGTH, 1, file);
    fwrite(&product->isPopular, IS_POPULAR_LENGTH, 1, file);
    fwrite(product->releaseDate, RELEASE_DATE_LENGTH, 1, file);
    fwrite(&product->stock, STOCK_LENGTH, 1, file);

    fclose(file);
}

Product* readProduct(FILE* file) {
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

    // Create a new product
    Product* product = createProduct(temp_serial_number, temp_brand, temp_type, temp_price, temp_isPopular, temp_releaseDate, temp_stock);

    // Free temporary memory
    free(temp_serial_number);
    free(temp_brand);
    free(temp_type);
    free(temp_releaseDate);

    return product;
}

void viewItemsMenu() {
    printf("1. Search By Text.\n");
    printf("2. Search By Number.\n");
    printf("3. Search By Equals.\n");
    printf("4. Search By Date.\n");
}

void viewItems() {
    int user_choice;
    printf("How would you want to view items?");

}