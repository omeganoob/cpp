#include <stdexcept>
#include <exception>
#include <functional>
#include <iostream>
#include <cstring>

using string = const char*;
#define red "\033[1;31m"
#define green "\033[1;32m"
#define end_color "\033[0m"

u_int32_t factorial( u_int32_t number ) {
    // return number <= 1 ? number : factorial( number - 1 ) * number;  // fail
    return number <= 1 ? 1 : factorial(number-1) * number;
}

void assert_that(bool statement, string msg)
{
    if(statement == false) throw std::runtime_error{ msg };
}

void run_test(std::function<void()> test_case, string test_name)
{
    try
    {
        test_case();
        std::cout << green << "[+] Test " << test_name << " passed."<< end_color << "\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << red <<"[+] Test " << test_name << " failed: " << end_color << e.what() << ".\n";
    }
}

// Interface
class Database {
public:
    virtual bool checkPassword(string username, string password) = 0;
    virtual ~Database() = default;
};

// Mock
class MockDatabase : public Database {
public:
    bool checkPassword(string username, string password) override {
        
        lastUsername = username;
        lastPassword = password;
        //Check server database and query here in real database
        return shouldReturn;
    }

    void setShouldReturn(bool value) {
        shouldReturn = value;
    }

    string getLastUsername() { return lastUsername; }
    string getLastPassword() { return lastPassword; }

private:
    string lastUsername = "";
    string lastPassword = "";
    bool shouldReturn = true;
};

// Real class being tested
class LoginService {
public:
    LoginService(Database& db) : db_(db) {}

    bool login(string username, string password) {
        if (strcmp(username, "") == 0 || strcmp(password, "") == 0) return false;
        return db_.checkPassword(username, password);
    }
private:
    Database& db_;
};

void empty_username_test()
{
    MockDatabase mockDb;
    LoginService service(mockDb);

    assert_that(service.login("", "pass") == false, "empty username should fail");
}

void empty_password_test()
{
    MockDatabase mockDb;
    LoginService service(mockDb);

    assert_that(service.login("user", "") == false, "empty password should fail");
}

void valid_login_test()
{
    MockDatabase mockDb;
    LoginService service(mockDb);

    mockDb.setShouldReturn(true);

    // Test successful login
    mockDb.setShouldReturn(true);
    assert_that(service.login("user1", "pass123") == true, "valid login should succeed");
    assert_that(strcmp(mockDb.getLastUsername(), "user1") == 0, "username should match");
    assert_that(strcmp(mockDb.getLastPassword(), "pass123") == 0, "password should match");
}

void failed_login_tets()
{
    MockDatabase mockDb;
    LoginService service(mockDb);
    mockDb.setShouldReturn(false);
    assert_that(service.login("user1", "wrong") == false, "invalid login should fail");
}

int main(int, char**)
{
    // run_test(fact_test, "fact test");

    run_test(empty_username_test, "Empty username");
    run_test(empty_password_test, "Empty password");
    run_test(valid_login_test, "Valid login");
    run_test(failed_login_tets, "Failed login");

    return 0;
}