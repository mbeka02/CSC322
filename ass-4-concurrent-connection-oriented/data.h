#include <stdbool.h>
#ifndef DATA_H
#define DATA_H
/**
 * Data structure to hold the data to be sent to the server
 
*/
struct Data
{
    int choice;         // Which functionality
    int m;              // Display catalogue
    int X;              // Display catalogue
    int z;              // Display catalogue
    char search[100];       // search for book
    int n; // Order a book
    char y[100];            // Order a book
    char x[100];            // Order a book
    int orderno;        // Pay for book
    double amount;      // Pay for book
    bool isPaymentSuccesful;
};

#endif
