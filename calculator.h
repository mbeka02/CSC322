#ifndef CALCULATOR_H
#define CALCULATOR_H


#include <stdio.h>
#include <math.h>
// Function prototypes
int add(int a, int b);
int multiply(int a, int b);
double square_root(int a);
int square(int a);
// Function to add two numbers
int add(int a, int b) {
    return a + b;
}

// Function to multiply two numbers
int multiply(int a, int b) {
    return a * b;
}

// Function to calculate the square root of a number
double square_root(int a) {
    return sqrt(a);
}

// Function to calculate the square of a number
int square(int a) {
    return a * a;
}
#endif
