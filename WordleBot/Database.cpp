#include"Database.h"

Database::Database() {
    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        system("pause");
        exit(1);
    }

    con->setSchema("wordle");
    return;
}

Database::~Database() {

}

void Database::addPlayer(uint64_t snowflake, std::string username) {
    pstmt = con->prepareStatement("INSERT INTO players(playerID,username) VALUES(?,?)");
    pstmt->setUInt64(1, snowflake);
    pstmt->setString(2, username);
    pstmt->execute();
    return;
}

void Database::updateStats(uint64_t snowflake, uint64_t guesses, int wordleNum) {
    res = stmt->executeQuery("SELECT guesses, possible FROM players WHERE playerID='" + std::to_string(snowflake) + "' ");
    res->next();
    float guessesf = (float)res->getUInt64("guesses") + (float)guesses;
    float possible = (float)res->getUInt64("possible") + 6.0;
    float ratio = possible / guessesf;
    pstmt = con->prepareStatement("UPDATE players SET guesses=guesses+(?), possible=possible+6, ratio=(?) WHERE playerID=(?)");
    pstmt->setUInt64(1, guesses);
    pstmt->setDouble(2, ratio);
    pstmt->setUInt64(3, snowflake);
    pstmt->execute();
    return;
}

void Database::updateWordleStats(int wordleNum, uint64_t guesses) {
    try {
        pstmt = con->prepareStatement("UPDATE wordles SET guesses=guesses+(?), possible=possible+6 WHERE wordleNum=(?)");
        pstmt->setUInt64(1, guesses);
        pstmt->setUInt64(2, wordleNum);
        pstmt->execute();
    }
    catch (sql::SQLException e) {
        std::cout << e.what() << std::endl;
    }
    return;
}

void Database::retrieveStats(uint64_t snowflake, statStruct& result) {
    try {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM players WHERE playerID='" + std::to_string(snowflake) + "' ");
        res->next();
        result.guesses = res->getUInt64("guesses");
        result.possible = res->getUInt64("possible");
        result.ratio = res->getDouble("ratio");
        result.username = res->getString("username");
    }
    catch (sql::SQLException e) {
        std::cout << e.what() << std::endl;
    }
    return;
}

std::vector<statStruct> Database::retrieveLeaderboard() {
    stmt = con->createStatement();
    statStruct stats;
    std::vector<statStruct> leaderboard;
    try
    {
        res = stmt->executeQuery("SELECT * FROM players ORDER BY ratio DESC LIMIT 10");
    }
    catch (const sql::SQLException& e)
    {
        std::cout << e.what() << std::endl;
    }
    while (res->next()) {
        stats.ratio = res->getDouble("ratio");
        stats.guesses = res->getUInt64("guesses");
        stats.possible = res->getUInt64("possible");
        stats.username = res->getString("username");
        stats.userID = res->getUInt64("playerID");
        leaderboard.push_back(stats);
    }
    return leaderboard;
}

void Database::addWordle(int wordleNum) {
    pstmt = con->prepareStatement("INSERT INTO wordles(wordleNum) VALUES(?)");
    pstmt->setUInt64(1, wordleNum);
    pstmt->execute();
    return;
}

bool Database::playerExists(uint64_t snowflake) {
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT guesses FROM players WHERE playerID='" + std::to_string(snowflake) + "'");
    if (res->next())
        return true;

    else
        return false;
}

bool Database::wordleExists(int wordleNum) {
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT guesses FROM wordles WHERE wordleNum='" + std::to_string(wordleNum) + "'");
    if (res->next())
        return true;

    else
        return false;
}

void Database::retrieveWordleStats(uint64_t stats[3]) {
    try {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM wordles ORDER BY wordleNum DESC");
        res->next();
        stats[0] = res->getInt("wordleNum");
        stats[1] = res->getUInt64("guesses");
        stats[2] = res->getUInt64("possible");
    }
    catch (const sql::SQLException& e)
    {
        std::cout << e.what() << std::endl;
    }
    return;
}