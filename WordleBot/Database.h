#pragma once
#include<iostream>
#include<vector>
#include<string>```
#include<jdbc/mysql_connection.h>
#include<jdbc/cppconn/driver.h>
#include<jdbc/cppconn/exception.h>
#include<jdbc/cppconn/prepared_statement.h>

typedef struct statStruct {
    std::string username = "";
    uint64_t guesses = 0;
    uint64_t possible = 0;
    float ratio = 0.0;
    uint64_t userID = 0;
}statStruct;

class Database {
public:
    Database();
    ~Database();
    bool wordleExists(int wordleNum);
    bool playerExists(uint64_t snowflake);
    void addWordle(int wordleNum);
    std::vector<statStruct> retrieveLeaderboard();
    void retrieveStats(uint64_t snowflake, statStruct& result);
    void updateWordleStats(int wordleNum, uint64_t guesses);
    void updateStats(uint64_t snowflake, uint64_t guesses, int wordleNum);
    void addPlayer(uint64_t snowflake, std::string username);
    void retrieveWordleStats(uint64_t stats[]);

private:
    const std::string server = "tcp://127.0.0.1:3306";
    const std::string username = "root";
    const std::string password = "D3lph12020";

    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;
};


