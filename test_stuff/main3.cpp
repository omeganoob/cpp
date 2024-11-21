#include <gtest/gtest.h>

uint32_t factorial( uint32_t number ) {
    // return number <= 1 ? number : factorial( number - 1 ) * number;  // fail
    return number <= 1 ? 1 : factorial(number-1) * number;
}

// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) {
  EXPECT_EQ(factorial(0), 1);
}

// Tests factorial of positive numbers.
TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(factorial(1), 1);
  EXPECT_EQ(factorial(2), 2);
  EXPECT_EQ(factorial(3), 6);
  EXPECT_EQ(factorial(8), 40320);
}