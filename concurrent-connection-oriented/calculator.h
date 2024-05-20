#ifndef CALCULATOR_H
#define CALCULATOR_H


#include <stdio.h>
#include <math.h>
// Function prototypes
int add(int a, int b);
int multiply(int a, int b);
double square_root(int a);
int square(int a);

struct Calculator_data{
  int choice;
  int a;
  int b;
};
#endif
