#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX_LINE_LENGTH 500

void printSameCharacterXTimes(char character, int times);
char *leftAlignSentence(char *sentence, int lengthOfLine);
void DisplayMenu(int lengthOfLine);
char *centeredSentence(char *sentence, int lengthOfLine);
char* searchInFile(const char *searchStr);
char* DisplayCatalog(int M, int x, int z);
char *SearchBook(char* string);
void PurchaseItem();
int getPriceOfItem(char *title);
void PayForItem();
int OrderBook(char *x , char *y , int n);

#endif
