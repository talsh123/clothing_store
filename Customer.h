#ifndef CUSTOMER_H
#define CUSTOMER_H

typedef struct {
    char* id;            // Unique identifier for the customer
    char* name;              // Customer's name
    char* joinDate;           // Date the customer joined (format: DD-MM-YYYY)
    float totalAmountSpent;  // Total amount the customer has spent
    int itemsPurchased;       // IDs of up to 3 items the customer purchased
} Customer;

#endif // CUSTOMER_H