/**
 * @file calculator.cpp
 * @brief A simple calculator program that performs basic arithmetic operations.
 *
 * This program allows the user to select up to two arithmetic operations from a predefined set
 * and apply them to two input numbers. The operations include addition, subtraction, multiplication,
 * division, finding the maximum, and finding the minimum.
 */

#include <iomanip>
#include <iostream>
#include <map>

// Define constants for each arithmetic operation using bitwise flags
#define CONG 0x0001  ///< Addition operation flag
#define TRU 0x0002   ///< Subtraction operation flag
#define NHAN 0x0004  ///< Multiplication operation flag
#define CHIA 0x0008  ///< Division operation flag
#define MAX 0x0010   ///< Maximum operation flag
#define MIN 0x0020   ///< Minimum operation flag

// Map to associate user input with the corresponding operation flag
std::map<uint, uint> mathOP { { 1, CONG }, { 2, TRU }, { 3, NHAN }, { 4, CHIA }, { 5, MIN }, { 6, MAX } };

/**
 * @brief Performs arithmetic operations on two numbers based on the specified operation type.
 *
 * @param MathType A bitmask representing the selected arithmetic operations.
 * @param num1 The first floating-point number for the operation.
 * @param num2 The second floating-point number for the operation.
 *
 * This function checks the MathType bitmask to determine which operations to perform on num1 and num2.
 * It supports addition, subtraction, multiplication, division, finding the maximum, and finding the minimum.
 * The results are printed to the standard output.
 */
void calculator(uint MathType, float num1, float num2)
{
    // Set output format to fixed with two decimal places
    std::cout << std::fixed << std::setprecision(2);

    // Perform addition if the CONG flag is set
    if (MathType & CONG)
        std::cout << "Addition result: " << num1 + num2 << std::endl;

    // Perform subtraction if the TRU flag is set
    if (MathType & TRU)
        std::cout << "Subtraction result: " << num1 - num2 << std::endl;

    // Perform multiplication if the NHAN flag is set
    if (MathType & NHAN)
        std::cout << "Multiplication result: " << num1 * num2 << std::endl;

    // Perform division if the CHIA flag is set, with a check for division by zero
    if (MathType & CHIA) {
        if (num2 != 0)
            std::cout << "Division result: " << num1 / num2 << std::endl;
        else
            std::cout << "Division by zero error!" << std::endl;
    }

    // Find the maximum if the MAX flag is set
    if (MathType & MAX)
        std::cout << "Maximum: " << (num1 > num2 ? num1 : num2) << std::endl;

    // Find the minimum if the MIN flag is set
    if (MathType & MIN)
        std::cout << "Minimum: " << (num1 < num2 ? num1 : num2) << std::endl;
}

/**
 * @brief Main function to execute the calculator program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return int Exit status of the program.
 *
 * The main function prompts the user to select arithmetic operations and input numbers.
 * It then calls the calculator function with the selected operations and numbers.
 */
int main(int argc, char** argv)
{
    uint num_op {};  // Number of operations selected by the user
    std::cout << "\n1.CONG\n"
                << "2.TRU\n"
                << "3.NHAN\n"
                << "4.CHIA\n"
                << "5.MIN\n"
                << "6.MAX\n";
    std::cout << "How many Operators? (max 2)";
    std::cin >> num_op;
    uint op1, op2;  // Variables to store selected operations
    float num1, num2;  // Variables to store input numbers

    // Handle case where the user selects two operations
    if (num_op == 2) {
        std::cin >> op1 >> op2;
        std::cout << "Enter numbers: ";
        std::cin >> num1 >> num2;
        calculator(mathOP[op1]|mathOP[op2], num1, num2);
    } 
    // Handle case where the user selects one operation
    else if (num_op == 1) {
        std::cin >> op1;
        std::cout << "Enter numbers: ";
        std::cin >> num1 >> num2;
        calculator(mathOP[op1], num1, num2);
    }
    // Handle unsupported number of operations
    else {
        std::cout << "Numbers of Operator not supported.\n";
    }
    return 0;
}