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

Item* createItem(char* serialNumber, char* brand, char* type, double price, int isPopular, char* releaseDate, int stock);
int compareitems(const Item* item1, const Item* item2);
void writeItem(Item* item, const char* fileName);
Item* readItem(FILE* file);
void viewItemsMenu();
void printItems(Item* items, int itemCount);
Item* findByBrandType(char* userBrand, char* userType, int filterType);
void searchByBrandOrType();
void viewItems();

#endif // ITEM_H