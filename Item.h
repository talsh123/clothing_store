// Tal-Shalom Ben Ovadia 322356346 
// Stav Moalem 211500657
// Rafi Erez 301420352

#ifndef ITEM_H
#define ITEM_H

typedef struct item {
    char* serialNumber; // 13 Characters
    char* brand; // 31 characters
    char* type; // 21 Characters - Shirt, Pants etc...
    double price;
    int isPopular;
    char* releaseDate; // DD-MM-YYYY Format, 11 characters
    int stock;
} Item;

typedef struct ItemNode {
    Item* item;
    struct ItemNode* next;
} ItemNode;

extern ItemNode* globalItems;

#define ITEMS_FILE "items.bin"

// Item FILE STRUCTURE
#define SERIAL_NUMBER_LENGTH 13
#define BRAND_LENGTH 31
#define TYPE_LENGTH 12
#define PRICE_LENGTH 8
#define IS_POPULAR_LENGTH 4
#define RELEASE_DATE_LENGTH 11
#define STOCK_LENGTH 4

ItemNode* createItem(char* serialNumber, char* brand, char* type, double price, int isPopular, char* releaseDate, int stock); // done
void writeItem(Item* item, FILE* file); // done
Item* readItem(FILE* file); // done
Item* getAllItems(); // done, didnt changed
void viewItemsMenu(); // upgraded menu to table
void printItems();
void printGivenItems(ItemNode* items);
void freeItem(Item* item);
ItemNode* findByBrandType(char* userBrand, char* userType, int filterType); // done
ItemNode* findByPrice(double price, char identifier); // done
ItemNode* findByStock(int stock, char identifier); // done
ItemNode* findByProperty(char* property, void* value); // done
ItemNode* findByDate(char* userDate, char identifier); // done
ItemNode* findDatesInRange(char* userDate1, char* userDate2); // done
ItemNode* findItemBySerialNumber(char* serialNumber); // new, for sell simplfy 
Item* updateItem(char* userSerialNumber, int property, void* value);
Item* removeItem(char* serialNumber);
Item* sellItem(char* itemSerialNumber, char* userCustomerID, int amount);
void sellItemMenu(); // done
void removeItemMenu(); // done
void updateItemMenu(); // done
void updateItemMenuWithSerialNumber(char* serialNumber);
ItemNode* searchByBrandOrType();
ItemNode* searchByPriceorStock();
ItemNode* searchByEquals();
ItemNode* searchByDate();
void viewItems();
void addNewItem();
ItemNode* loadItemsLinkedList(const char* filename); // new
void saveItemsFromLinkedList(const char* fileName); // new
void addItemToList(ItemNode* newNode); // new
void freeItems();
ItemNode* mergeSortItems(ItemNode* head);
static void frontBackSplit(ItemNode* source, ItemNode** frontRef, ItemNode** backRef);
static ItemNode* mergeSortedLists(ItemNode* a, ItemNode* b);
void returnItemMenu();

#endif // ITEM_H