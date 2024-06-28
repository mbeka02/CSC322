#include <stdio.h>

int main() {
 
   char input[100];
  printf("Enter a string with spaces: ");
    scanf("%99[^\n]", input);  // Read up to 99 characters to avoid buffer overflow

    printf("You entered: %s\n", input);

    return 0;
}
