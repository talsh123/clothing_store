#ifndef CUSTOMER_H
#define CUSTOMER_H

typedef struct {
    char* id;            // Unique identifier for the customer
    char* name;              // Customer's name
    char* joinDate;           // Date the customer joined (format: DD-MM-YYYY)
    double totalAmountSpent;  // Total amount the customer has spent
    int itemsPurchased;       // IDs of up to 3 items the customer purchased
} Customer;

#define CUSTOMERS_FILE "customers.txt"

// Employee FILE properties LIMIT
#define ID_LENGTH 10 // 9 + 1
#define NAME_LENGTH 64 
#define JOIN_DATE_LENGTH 11 //DD-MM-YYYY + 1
#define TOTAL_AMOUNT_SPENT_LENGTH 8 // Double
#define ITEMS_PURCHASED_LENGTH 4 // Integer

Customer* createCustomer(char* id, char* name, char* joinDate, double totalAmountSpent, int itemsPurchased);
void writeCustomer(Customer* customer, const char* fileName);
void writeCustomers(Customer* customers, int customerCount, const char* fileName);
Customer* readCustomer(FILE* file);
void printCustomers(Customer* customers, int customerCount);
Customer* getAllCustomers(int* customerCount);
int checkIfCustomerFileExists();
Customer* findCustomersByProperty(char* property, void* value);
Customer* findCustomersByID(char* userID);
Customer* removeCustomer(char* id);
Customer* updateCustomer(char* id, int property, void* value);
void removeCustomerMenu();
void addNewCustomer();
void checkCustomerPurchases();

#endif // CUSTOMER_H