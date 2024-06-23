#ifndef interface
#define interface
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 500
char* defaultSearchStringResult = "Book Not Found";

char *SearchBook(char* string){
  const char *filePath = "books.txt";

  FILE *filePtr=fopen(filePath,"r");
  if (filePtr ==NULL){
    perror("Error opening file");
  }
  char line[MAX_LINE_LENGTH];
  while(fgets(line,MAX_LINE_LENGTH,filePtr)!=NULL){
    if(strstr(line,string)!=NULL){
       fclose(filePtr);
            // Allocate memory for the matching line and copy it.
            char *result = (char *)malloc(strlen(line) + 1);
            strcpy(result, line);
            return result;
    }
  }

  return defaultSearchStringResult;
}

int OrderBook(char *x , char *y , int n){
  printf("Ordering %d copies of %s by %s\n",n,x,y);

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
  
  if(sprintf(orderFileRecord, "%s %d\n", x, n) < 0) {
    perror("Error formatting order record");
  }
  fputs(orderFileRecord, orderFile);
  fclose(orderFile);
  // Get order number
  char buffer[256];
  int line = 0;
  orderFile=fopen("orders.txt","r");
  while (fgets(buffer, 256, orderFile) != NULL) {
    printf("A new line found\n");
    line ++; 
  }
  printf("%s in buffer\n", buffer);
  fclose(orderFile);
  
  // Return order number
  return line;
}
// TO DO : WORK ON THIS FUNCTION , CURRENT IMPLEMENTATION IS A PLACEHOLDER
int PayForBook(int orderno, float Amount){
  printf("The order number is : %d  and the amount is : %f",orderno,Amount );
  return 1;
}
char* DisplayCatalog(int M, int x, int z) {
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

    char* returnString = (char *) malloc(sizeof(char) * 100 * numberOfFileEntries);

    // Store content of file
    int contentSize = 1000;
    char fileContent[contentSize];

    // Go to result X of file
    int lineNumber = 0;
    while(fgets(fileContent, contentSize, filePtr)) {
        if (lineNumber == 0) {
            printf("Ignoring header file!\n");
        } else if (lineNumber < x) {
            printf("Ignore file content: %s", fileContent);
        } else if (lineNumber >= x && numberOfFileEntries > 0){
            strcat(returnString, fileContent);
            numberOfFileEntries--;
        } else {
            printf("Ignore file because it's out of range!\n");
        }
        lineNumber++;
    }

    fclose(filePtr);
    // Return results
    return returnString;
}
#endif
