#include "helpers.h"
/*
   A book has Title, Author(s), ISBN,
   Publisher, Date of publication
   */
typedef struct {
    char **array;
    size_t used;
    size_t size;

} Array;

void initArray (Array *a, size_t initialSize){
    a->array = (char **)malloc(initialSize * sizeof(char *));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, char *element) {
    if (a->used == a->size) {
        a->size *= 2;
        a->array = (char **) realloc(a->array, a->size * sizeof(char *));
    }
    a->array[a->used++] = element;
}

void freeArray(Array *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

Array purchasedItems;

char *centeredSentence(char *sentence, int lengthOfLine) {
    if (strlen(sentence) >= lengthOfLine) {
        return sentence;
    }

    char space = ' ';
    char *finalString = (char *) malloc(sizeof(char) * lengthOfLine);
    int lengthOfSpaces = lengthOfLine - strlen(sentence);

    for (int i = 0; i < lengthOfLine; i++) {
        // First half add spaces in beginning
        if (i < lengthOfSpaces / 2) {
            finalString[i] = space;
        } else {
            // Second half I add what sentence had and string
            if (i < lengthOfSpaces / 2 + strlen(sentence)) {
                finalString[i] = sentence[i - lengthOfSpaces / 2];
            } else {
                finalString[i] = space;
            }
        }
    }

    return finalString;
}

char* searchInFile(const char *searchStr) {
    printf("Searching for %s\n", searchStr);
    const char *filePath = "books.txt";
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, searchStr) != NULL) {

            fclose(file);
            // Allocate memory for the matching line and copy it.
            char *result = (char *) malloc(strlen(line) + 1);
            strcpy(result, line);
            return result;
        }
    }

    fclose(file);
    return NULL;
}

char* DisplayCatalog(int M, int X, int z) {
    // Read the text file
    FILE *filePtr = fopen("./books.txt", "r");
    if (filePtr == NULL) {
        printf("File could not be opened!... Exiting program.\n");
    }

    // Get M entries as long X + M < Z
    int numberOfFileEntries = 0;
    if (X + M < z) {
        numberOfFileEntries = M;
    } else {
        // Otherwise get until Z
        numberOfFileEntries = z - X;
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
        } else if (lineNumber < X) {
            printf("Ignore file content: %s", fileContent);
        } else if (lineNumber >= X && numberOfFileEntries > 0){
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

/*
 *  M = max books to display
 *  x = start index of results
 *  z = end index of results
 * */
void DisplayMenu(int lengthOfLine) {
    printSameCharacterXTimes('-', lengthOfLine);
    printf("|%s|\n", centeredSentence("OPTIONS", lengthOfLine - 2));
    printSameCharacterXTimes('-', lengthOfLine);
    printf("|%s|\n", centeredSentence(" ", lengthOfLine - 2));
    printf("| %s|\n", leftAlignSentence("1. Display Catalog", lengthOfLine - 3));
    printf("| %s|\n", leftAlignSentence("2. Search For Item", lengthOfLine - 3));
    printf("| %s|\n", leftAlignSentence("3. Purchase Item", lengthOfLine - 3));
    printf("| %s|\n", leftAlignSentence("4. Pay For Item", lengthOfLine - 3));
    printf("| %s|\n", leftAlignSentence("5. Exit Program", lengthOfLine - 3));
    printf("|%s|\n", centeredSentence(" ", lengthOfLine - 2));
    printSameCharacterXTimes('-', lengthOfLine);
}

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
            if(result==NULL){
            perror("memory allocation failed");
            return NULL;
      }
            strcpy(result, line);
            return result;
    }
  }

  return "";
}
/*
 *  x => title
 *  y => ISBN
 *  n => number of books to order
 * */
int OrderBook(char *x, char *y, int n){
  // Open the file
  const char *filePath = "books.txt";
  FILE *filePtr=fopen(filePath,"r");
  if (filePtr ==NULL){
    perror("Error opening file");
  }


  char line[MAX_LINE_LENGTH];
  while(fgets(line,MAX_LINE_LENGTH,filePtr)!=NULL){
    if(strstr(line,x)!=NULL && strstr(line,y)!=NULL){
       fclose(filePtr);
            // Allocate memory for the matching line and copy it.
            char *result = (char *)malloc(strlen(line) + 1);
            if(result==NULL){
            perror("memory allocation failed");
            return -1;
      }
            strcpy(result, line);
            printf("The use has ordered %d copies of %s",n,result);
            return rand();
    }
  }

return -1;
  
}
bool PayForBook(int orderno, float Amount){
  printf("The order number is : %d  and the amount is : %f",orderno,Amount );
  return true;
}  
void PurchaseItem() {
    // display the catalog
    DisplayCatalog(1, 1, 2);

    char title[100];
    // ask the user to enter the title of the book they want to purchase
    printf("Enter the Title of the book you want to purchase: ");
    scanf("%s", title);

    // search for the book
    char *result = searchInFile(title);

    if (strcmp(result, "") == 0) {
        printf("Book not found!\n");
    } else {
        // Add book to list
        insertArray(&purchasedItems, result);
        printf("Book Purchased!\n");
    }
}

int getPriceOfItem(char *title) {
    return rand() % 100 + 1;
}

void PayForItem() {
    int amount = 0;

    for (int i = 0; i<purchasedItems.used; i++){
        int itemPrice = getPriceOfItem(purchasedItems.array[i]);
        amount += itemPrice;
    }
    printf("You have to pay Ksh %d. \n", amount);
    printf("Do you want to proceed with payment? (y/n): ");

    char choice;
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
        printf("Success! Book purchased.\n");
    } else {
        printf("Payment cancelled.\n");
    }
}

char *leftAlignSentence(char *sentence, int lengthOfLine) {
    if (strlen(sentence) >= lengthOfLine) {
        return sentence;
    }

    char space = ' ';
    char *finalString = (char *) malloc(sizeof(char) * lengthOfLine);

    // Place space at the end of sentence till its length is that of length of
    // line
    for (int i = 0; i < lengthOfLine; i++) {
        if (i < strlen(sentence)) {
            finalString[i] = sentence[i];
        } else {
            finalString[i] = space;
        }
    }

    return finalString;
}

void printSameCharacterXTimes(char character, int times) {
    for (int i = 0; i < times; i++) {
        printf("%c", character);
    }
    printf("\n");
}
