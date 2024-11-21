#include <iostream>
#include <string>
#include <functional>

#define red "\033[1;31m"
#define green "\033[1;32m"
#define end_color "\033[0m"

using TestCase = std::function<void()>;

void assert_that(bool statement, std::string msg);
void run_test(TestCase test_case, std::string test_name);

int square_area(int a)
{
    if(a < 0)
    {
        return 0;
    }
    return a*a;
}

void test_length_greater_than_0()
{
    int a = -2;
    assert_that(square_area(a) == 0, "length parameter less than or equal to 0");
}

int main(int, char**)
{
    std::cout << "--------------\n";

    run_test(test_length_greater_than_0, "Length value");

    return 0;
}

void assert_that(bool statement, std::string msg)
{
    if(statement == false)
    {
        throw std::runtime_error{ msg };
    }
}

void run_test(TestCase test_case, std::string test_name)
{
    try
    {
        test_case();
        std::cout << green <<"[+] Test " << test_name << " passed." << end_color << "\n"; 
    }
    catch(const std::exception& e)
    {
        std::cerr << red << "[-] Test " << test_name << " failed: " << end_color << e.what() << '\n';
    }
    
}