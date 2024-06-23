#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX_LINE_LENGTH 1024

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
    a->array = realloc(a->array, a->size * sizeof(char *));
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
  char *finalString = malloc(sizeof(char) * lengthOfLine);
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

char *leftAlignSentence(char *sentence, int lengthOfLine) {
  if (strlen(sentence) >= lengthOfLine) {
    return sentence;
  }

  char space = ' ';
  char *finalString = malloc(sizeof(char) * lengthOfLine);

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

/*ISBN number is unqiue and should be the value used to search for a book.
 */
/*
A book has Title, Author(s), ISBN, Publisher, Date of publication
*/
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
            char *result = malloc(strlen(line) + 1);
            strcpy(result, line);
            return result;
        }
    }

    fclose(file);
    return NULL;
}

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

void DisplayCatalog() {
  // Read the text file
  FILE *filePtr = fopen("./books.txt", "r");
  if (filePtr == NULL) {
    printf("File could not be opened!... Exiting program.\n");
  }

  // Store content of file
  int contentSize = 100;
  char fileContent[contentSize];

  // Print every line in the file
  while (fgets(fileContent, contentSize, filePtr)) {
    printf("%s", fileContent);
  }

  fclose(filePtr);
}

void PurchaseItem() {
  // display the catalog
  DisplayCatalog();

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
}atoi( )

int main(void) {
  int menu_item;
  int lengthOfLine = 50;
  initArray(&purchasedItems, 2);

  // Display list/menu of options
  DisplayMenu(lengthOfLine);
  bool stopLooping = false;
  
  while(true) {
    printf("Select an item from the menu: ");
    // Get user input
    scanf("%d", &menu_item);
    // switch case for each option
    switch (menu_item) {
    case 1:
      printf("...displaying catalog\n");
      DisplayCatalog();
      break;
    case 2:
      printf("Enter the title of the book you want to search for: ");
      char title[100];
      scanf("%s", title);

      char* search_result = searchInFile(title);
      if (strcmp(search_result, "") == 0) {
        printf("Book Not Found\n");
      } else {
        printf("Book Found!\n");
      }

      break;
    case 3:
      PurchaseItem();
      break;
    case 4:
      PayForItem();
      break;
    case 5:
      printf("Thanks for shopping with us\n");
      stopLooping = true;
      break;
    default:
      printf("Invalid option\n");
      break;
      
  }

    if (stopLooping == true) {
      break;
    }
}

  /*char *sentence = "Hello World!";
  printf("%s\n", centeredSentence(sentence, lengthOfLine));
  printf("%lu\n", strlen(centeredSentence(sentence, lengthOfLine)));*/

  freeArray(&purchasedItems);
  return 0;
}
