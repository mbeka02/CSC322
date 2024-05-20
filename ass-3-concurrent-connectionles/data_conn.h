#ifndef DATA
#define DATA
/**
 * Data structure to hold the data to be sent to the server
 
*/
struct Data
{
    int choice;         // Which functionality
    int m;              // Display catalogue
    int x;              // Display catalogue
    int z;              // Display catalogue
    char *search;       // search for book
    int number_ordered; // Order a book
    char *y;            // Order a book
    char *n;            // Order a book
    int orderno;        // Pay for book
    double amount;      // Pay for book
};

#endif