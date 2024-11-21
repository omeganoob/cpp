#include <gtest/gtest.h>
#include <sqlite3.h>
#include <string>
#include <memory>
#include <chrono>
#include <ctime>

// UserDatabase class to handle database operations
class UserDatabase {
private:
    sqlite3* db;

public:
    UserDatabase() {
        int rc = sqlite3_open(":memory:", &db);
        if (rc) {
            throw std::runtime_error("Can't open database");
        }
        
        // Create users table
        const char* sql = "CREATE TABLE users ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                         "username TEXT UNIQUE,"
                         "created_at TEXT)";
        
        char* errMsg = 0;
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            std::string error = errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error("SQL error: " + error);
        }
    }

    ~UserDatabase() {
        if (db) {
            sqlite3_close(db);
        }
    }

    int addUser(const std::string& username) {
        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::ctime(&now_time);

        // Prepare SQL statement
        const char* sql = "INSERT INTO users (username, created_at) VALUES (?, ?)";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, timestamp.c_str(), -1, SQLITE_STATIC);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        if (rc != SQLITE_DONE) {
            throw std::runtime_error("Failed to insert user");
        }

        return sqlite3_last_insert_rowid(db);
    }

    bool getUser(int userId, std::string& username, std::string& createdAt) {
        const char* sql = "SELECT username, created_at FROM users WHERE id = ?";
        sqlite3_stmt* stmt;
        
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        sqlite3_bind_int(stmt, 1, userId);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            sqlite3_finalize(stmt);
            return true;
        }
        
        sqlite3_finalize(stmt);
        return false;
    }
};

// UserService class for business logic
class UserService {
private:
    std::shared_ptr<UserDatabase> db;

public:
    UserService(std::shared_ptr<UserDatabase> database) : db(database) {}

    int registerUser(const std::string& username) {
        if (username.length() < 3) {
            throw std::invalid_argument("Username must be at least 3 characters long");
        }
        return db->addUser(username);
    }

    bool getUserDetails(int userId, std::string& username, std::string& createdAt) {
        return db->getUser(userId, username, createdAt);
    }
};

// Test fixture for integration tests
class UserIntegrationTest : public ::testing::Test {
protected:
    std::shared_ptr<UserDatabase> db;
    std::unique_ptr<UserService> userService;

    void SetUp() override {
        db = std::make_shared<UserDatabase>();
        userService = std::make_unique<UserService>(db);
    }

    void TearDown() override {
        userService.reset();
        db.reset();
    }
};

// Integration tests
TEST_F(UserIntegrationTest, RegisterAndRetrieveUser) {
    // Test user registration
    const std::string username = "testuser";
    int userId = userService->registerUser(username);
    ASSERT_GT(userId, 0);

    // Test user retrieval
    std::string retrievedUsername, createdAt;
    bool found = userService->getUserDetails(userId, retrievedUsername, createdAt);
    
    ASSERT_TRUE(found);
    EXPECT_EQ(retrievedUsername, username);
    EXPECT_FALSE(createdAt.empty());
}

TEST_F(UserIntegrationTest, InvalidUsername) {
    // Test validation across components
    const std::string shortUsername = "ab";
    EXPECT_THROW(userService->registerUser(shortUsername), std::invalid_argument);
}

// Main function
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}