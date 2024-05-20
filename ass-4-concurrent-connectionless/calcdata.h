#ifndef CALC_H
#define CALC_H


/**
 * 
 * This struct is used to store the data that is sent to the server
*/
struct Calc_data{
    int choice; // 1 for sum, 2 for multiplication, 3 for square, 4 for square root
    int sum_1; // first number for sum
    int sum_2; // second number for sum
    int mult_1; // first number for multiplication
    int mult_2; // second number for multiplication
    int sqrt_value; // value for square root
    int square_value; // value for square
};


#endif