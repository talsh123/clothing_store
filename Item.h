#ifndef ITEM_H
#define ITEM_H
#include "Customer.h"

typedef struct item {
    char* serialNumber; // 13 Characters
    char* brand; // 31 characters
    char* type; // 21 Characters - Shirt, Pants etc...
    double price;
    int isPopular;
    char* releaseDate; // DD-MM-YYYY Format, 11 characters
    int stock;
} Item;

#define ITEMS_FILE "items.bin"

// Item FILE STRUCTURE
#define SERIAL_NUMBER_LENGTH 13
#define BRAND_LENGTH 31
#define TYPE_LENGTH 12
#define PRICE_LENGTH 8
#define IS_POPULAR_LENGTH 4
#define RELEASE_DATE_LENGTH 11
#define STOCK_LENGTH 4

Item* createItem(char* serialNumber, char* brand, char* type, double price, int isPopular, char* releaseDate, int stock);
void writeItem(Item* item, FILE* file);
void writeItems(Item* items, int itemCount, const char* fileName);
Item* readItem(FILE* file);
Item* getAllItems(int* itemCount);
void viewItemsMenu();
void printItems(Item* items, int itemCount);
Item* findByBrandType(char* userBrand, char* userType, int filterType);
Item* findByPrice(double price, char identifier);
Item* findByStock(int stock, char identifier);
Item* findByProperty(char* property, void* value);
Item* findByDate(char* userDate, char identifier);
Item* findDatesInRange(char* userDate1, char* userDate2);
Item* updateItem(char* userSerialNumber, char* property, void* value);
Item* removeItem(char* serialNumber);
void rewriteCustomer(char* customerID, Customer* updatedCustomer);
Item* sellItem(char* itemSerialNumber, char* userCustomerID, int amount);
void returnItemMenu();
void sellItemMenu();
void removeItemMenu();
void updateItemMenu();
void searchByBrandOrType();
void searchByPriceorStock();
void searchByEquals();
void searchByDate();
void viewItems();
void addNewItem();

#endif // ITEM_H