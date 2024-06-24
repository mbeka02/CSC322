#ifndef interface
#define interface
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 500
char *defaultSearchStringResult = "Book Not Found";

char *SearchBook(char *string) {
  const char *filePath = "books.txt";

  FILE *filePtr = fopen(filePath, "r");
  if (filePtr == NULL) {
    perror("Error opening file");
  }
  char line[MAX_LINE_LENGTH];
  while (fgets(line, MAX_LINE_LENGTH, filePtr) != NULL) {
    if (strstr(line, string) != NULL) {
      fclose(filePtr);
      // Allocate memory for the matching line and copy it.
      char *result = (char *)malloc(strlen(line) + 1);
      strcpy(result, line);
      return result;
    }
  }

  return defaultSearchStringResult;
}

int OrderBook(char *x, char *y, int n) {
  // See if book exists
  int doesBookExist = strcmp(SearchBook(x), defaultSearchStringResult);
  if (doesBookExist == 0) {
    // Book does not exist
    printf("Book %s does not exist!", x);
    return -1;
  }

  // Store book in orders list if it exists
  FILE *orderFile = fopen("orders.txt", "a+");
  if (orderFile == NULL) {
    perror("Error opening file");
    return -1;
  }
  char orderFileRecord[100];

  if (sprintf(orderFileRecord, "%s %d\n", x, n) < 0) {
    perror("Error formatting order record");
  }
  fputs(orderFileRecord, orderFile);
  fclose(orderFile);
  // Get order number
  char buffer[256];
  int line = 0;
  orderFile = fopen("orders.txt", "r");
  while (fgets(buffer, 256, orderFile) != NULL) {
    line++;
  }
  fclose(orderFile);

  // Return order number
  return line;
}

int getAmountOfBooks(char *order) {
    int posOfAmount = 0;
    // Go to end of string
    for (int i = strlen(order); i >= 0; i--) {
        if (order[i] == ' ') {
            posOfAmount = i;
            break;
        }
    }

    char number[100];
    strncpy(number, order+(posOfAmount + 1), strlen(order));
    
    return atoi(number);
}

void *getBookTitle(char *order, char *returnStr) {
    int posOfAmount = 0;
    // Go to end of string
    for (int i = 0; i < strlen(order); i++) {
        if (order[i] == ' ') {
            posOfAmount = i;
            break;
        }
    }

    strncpy(returnStr, order+(0), posOfAmount);

    return returnStr;
}

// TO DO : WORK ON THIS FUNCTION , CURRENT IMPLEMENTATION IS A PLACEHOLDER
int PayForBook(int orderno, float Amount) {
    int line = 0;
    char buffer[256];
    FILE* orderFile = fopen("orders.txt", "r");
    
    // Check if the order exists
    while(fgets(buffer, 256, orderFile) != NULL) {
        if (line == orderno) {
            // Order exists
            break;
        }
        line++;
    }

    if (line < orderno) {
        perror("Book does not exist!\n");
        return -1;
    }
    
    // We get the book and number of books
    int numBooks = getAmountOfBooks(buffer);

    // We get price of book
    char searchBook[100];
    getBookTitle(buffer, searchBook);

    // We multiply price and number of books
    char *bookSearch = SearchBook(searchBook);

    int bookPrice = getAmountOfBooks(bookSearch);
    return bookPrice * numBooks;
}

/*
 * M - max number of books
 * x - lower bound (from)
 * z - upper bounde (to)
 * */
void DisplayCatalog(int M, int x, int z, char *returnString) {
  // Read the text file
  FILE *filePtr = fopen("./books.txt", "r");
  if (filePtr == NULL) {
    printf("File could not be opened!... Exiting program.\n");
  }

  // Get M entries as long X + M < Z
  int numberOfFileEntries = 0;
  if (x + M < z) {
    numberOfFileEntries = M;
  } else {
    // Otherwise get until Z
    numberOfFileEntries = z - x;
  }

  // Store content of file
  int contentSize = 1000;
  char fileContent[contentSize];

  // Go to result X of file
  int lineNumber = 0;
  while (fgets(fileContent, contentSize, filePtr)) {
    if (lineNumber == 0) {
      printf("Ignoring header file!\n");
    } else if (lineNumber < x) {
      printf("Ignore file content: %s", fileContent);
    } else if (lineNumber >= x && numberOfFileEntries > 0) {
      strcat(returnString, fileContent);
      numberOfFileEntries--;
    } 
    lineNumber++;
  }

  fclose(filePtr);
}
#endif
