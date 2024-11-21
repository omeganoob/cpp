#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>

// Interface: DatabaseInterface
class DatabaseInterface {
public:
    virtual ~DatabaseInterface() = default;
    virtual bool connect(const std::string& username, const std::string& password) = 0;
    virtual bool isConnected() const = 0;
};

// Application class that depends on DatabaseInterface
class Application {
    DatabaseInterface& database_;

public:
    explicit Application(DatabaseInterface& database) : database_(database) {}

    bool login(const std::string& username, const std::string& password) {
        if (!database_.connect(username, password)) {
            return false;
        }
        return database_.isConnected();
    }
};

// Mock class for DatabaseInterface
class MockDatabase : public DatabaseInterface {
public:
    MOCK_METHOD(bool, connect, (const std::string& username, const std::string& password), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
};

// Test case for Application::login using Google Mock
TEST(ApplicationTest, LoginSuccessful) {
    MockDatabase mockDatabase;
    Application app(mockDatabase);

    // Set expectations on the mock object
    EXPECT_CALL(mockDatabase, connect("user", "pass")).WillOnce(testing::Return(true));
    EXPECT_CALL(mockDatabase, isConnected()).WillOnce(testing::Return(true));
    // Call the method and check the result
    EXPECT_TRUE(app.login("user", "pass"));
}

TEST(ApplicationTest, LoginFailsOnWrongCredentials) {
    MockDatabase mockDatabase;
    Application app(mockDatabase);

    // Set expectations on the mock object
    EXPECT_CALL(mockDatabase, connect("user", "wrongpass")).WillOnce(testing::Return(false));

    // Call the method and check the result
    EXPECT_FALSE(app.login("user", "wrongpass"));
}

// Main function to run tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}