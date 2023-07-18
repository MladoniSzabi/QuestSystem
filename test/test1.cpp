#include <sstream>
#include <gtest/gtest.h>
#include <sqlite/sqlite3.h>
#include "QuestSystem.hpp"
#include "Quest.hpp"

typedef std::vector<std::vector<std::string>> SqlReturn;

extern int callback(void *retvalPtr, int columnCount, char **values, char **columnName);

class QuestSystemFixture : public ::testing::Test
{
private:
protected:
    sqlite3 *db;
    QuestSystem qs;
    void SetUp() override
    {

        char *errmsg;

        // Open sharable inmemory database
        int rc = sqlite3_open(":memory:", &db);
        ASSERT_EQ(rc, 0) << sqlite3_errmsg(db);

        std::stringstream sql;
        sql << "CREATE TABLE Quest(Id INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT UNIQUE NOT NULL, Description TEXT);"
            << "CREATE TABLE Stage(Id INTEGER PRIMARY KEY AUTOINCREMENT, QuestId INTEGER NOT NULL, Description TEXT,"
            << "    Level INTEGER, FOREIGN KEY (QuestId) REFERENCES Quest(Id));"
            << "CREATE TABLE Progress(Id INTEGER PRIMARY KEY AUTOINCREMENT, StageId INTEGER NOT NULL, Finished INTEGER,"
            << "    FOREIGN KEY (StageId) REFERENCES Stage(Id));"
            << "CREATE TABLE Quest_Requirements(Id INTEGER PRIMARY KEY AUTOINCREMENT, QuestId INTEGER NOT NULL, Item TEXT,"
            << "    Operand INTEGER, Value REAL, FOREIGN KEY (QuestId) REFERENCES Quest(Id));"
            << "CREATE TABLE Stage_Requirements(Id INTEGER PRIMARY KEY AUTOINCREMENT, StageId INTEGER NOT NULL, Item TEXT,"
            << "    Operand INTEGER, Value REAL, FOREIGN KEY (StageId) REFERENCES Stage(Id));"
            << "INSERT INTO Quest(Name, Description) VALUES ('Hello, World!', 'This is the first quest you will go on!'), ('Goodbye, Wold!', 'Test'), ('Quest 3', 'Quest');"
            << "INSERT INTO Stage(QuestId, Description, Level) VALUES (1, 'Step 1', 0), (1, 'Step 2', 1), (2, 'Test1', 0);"
            << "INSERT INTO Quest_Requirements(QuestId, Item, Operand, Value) VALUES (1, 'level', " << std::to_string((int)Operand::GREATER_THAN_OR_EQUAL) << ", 2);"
            << "INSERT INTO Quest_Requirements(QuestId, Item, Operand, Value) VALUES (2, 'level', " << std::to_string((int)Operand::GREATER_THAN_OR_EQUAL) << ", 3);"
            << "INSERT INTO Quest_Requirements(QuestId, Item, Operand, Value) VALUES (2, 'place', " << std::to_string((int)Operand::EQUAL) << ", 1);"
            << "INSERT INTO Stage_Requirements(StageId, Item, Operand, Value) VALUES (1, 'roaches', " << std::to_string((int)Operand::GREATER_THAN_OR_EQUAL) << ", 10);"
            << "INSERT INTO Stage_Requirements(StageId, Item, Operand, Value) VALUES (2, 'roaches', " << std::to_string((int)Operand::GREATER_THAN_OR_EQUAL) << ", 20);"
            << "INSERT INTO Stage_Requirements(StageId, Item, Operand, Value) VALUES (2, 'deaths', " << std::to_string((int)Operand::LESS_THAN) << ", 3);";

        // Create Quest Database
        rc = sqlite3_exec(
            db,
            sql.str().c_str(),
            nullptr,
            nullptr,
            &errmsg);

        EXPECT_EQ(errmsg, nullptr) << errmsg;
        ASSERT_EQ(rc, 0);

        // Pass the database to the quest system
        qs.setDatabase(db);
    }

    void TearDown() override
    {
        qs.close();
    }
};

TEST_F(QuestSystemFixture, TestStartQuest)
{
    EXPECT_EQ(qs.getActiveQuests().size(), 0);
    const char *error = qs.startQuest(1);
    EXPECT_EQ(error, nullptr) << error;
    EXPECT_EQ(qs.getActiveQuests().size(), 1);
    std::vector<Stage> as = qs.getActiveStages();
    EXPECT_EQ(as.size(), 1);
    EXPECT_EQ(as[0].id, 1);
    EXPECT_EQ(as[0].description, "Step 1");
    error = qs.startQuest("Goodbye, Wold!");
    EXPECT_EQ(error, nullptr) << error;
    EXPECT_EQ(qs.getActiveQuests().size(), 2);
    EXPECT_EQ(qs.getActiveStages().size(), 2);
    std::cout << std::endl;
    std::cout << std::endl;
}

TEST_F(QuestSystemFixture, TestGetQuests)
{
    EXPECT_EQ(qs.getAllQuests().size(), 3);
}

TEST_F(QuestSystemFixture, TestGetStageForQuest)
{
    std::vector<Stage> s = qs.getStagesForQuest(1);
    for (auto &stage : s)
    {
        EXPECT_EQ(stage.questId, 1);
    }

    EXPECT_EQ(s.size(), 2);
}

TEST_F(QuestSystemFixture, TestAvailableQuests)
{
    std::unordered_map<std::string, double> info;
    std::vector<Quest> q;
    q = qs.getAvailableQuests(info);
    EXPECT_EQ(q.size(), 1);
    EXPECT_FALSE(qs.isQuestAvailable(1, info));
    EXPECT_FALSE(qs.isQuestAvailable(2, info));
    info["level"] = 2;
    q = qs.getAvailableQuests(info);
    EXPECT_EQ(q.size(), 2);
    EXPECT_EQ(q[0].id, 1);
    EXPECT_TRUE(qs.isQuestAvailable(1, info));
    EXPECT_FALSE(qs.isQuestAvailable(2, info));
    info["level"] = 3;
    info["place"] = 0;
    q = qs.getAvailableQuests(info);
    EXPECT_EQ(q.size(), 2);
    EXPECT_EQ(q[0].id, 1);
    EXPECT_TRUE(qs.isQuestAvailable(1, info));
    EXPECT_FALSE(qs.isQuestAvailable(2, info));
    info["place"] = 1;
    q = qs.getAvailableQuests(info);
    EXPECT_EQ(q.size(), 3);
    EXPECT_EQ(q[0].id, 1);
    EXPECT_EQ(q[1].id, 2);
    EXPECT_TRUE(qs.isQuestAvailable(1, info));
    EXPECT_TRUE(qs.isQuestAvailable(2, info));
}

TEST_F(QuestSystemFixture, TestStageCompleted)
{
    std::unordered_map<std::string, double> info;
    std::vector<Stage> s;
    EXPECT_FALSE(qs.isStageCompletable(1, info));
    EXPECT_FALSE(qs.isStageCompletable(2, info));
    s = qs.completeStage(1, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 0);
    s = qs.completeStage(2, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 0);

    info["roaches"] = 10;
    EXPECT_TRUE(qs.isStageCompletable(1, info));
    EXPECT_FALSE(qs.isStageCompletable(2, info));
    s = qs.completeStage(1, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 2);
    s = qs.completeStage(2, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 0);

    info["roaches"] = 20;
    EXPECT_TRUE(qs.isStageCompletable(1, info));
    EXPECT_FALSE(qs.isStageCompletable(2, info));
    s = qs.completeStage(1, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 2);
    s = qs.completeStage(2, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 0);

    info["deahs"] = 3;
    EXPECT_TRUE(qs.isStageCompletable(1, info));
    EXPECT_FALSE(qs.isStageCompletable(2, info));
    s = qs.completeStage(1, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 2);
    s = qs.completeStage(2, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 0);

    info["deaths"] = 1;
    EXPECT_TRUE(qs.isStageCompletable(1, info));
    EXPECT_TRUE(qs.isStageCompletable(2, info));
    s = qs.completeStage(1, info);
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s[0].id, 2);
    s = qs.completeStage(2, info);
    EXPECT_EQ(s.size(), 0);

    {
        std::string sql = "SELECT Id FROM Progress WHERE StageId=1 AND Finished=1;";
        SqlReturn sqlReturn;
        char *errorStr = nullptr;
        int errorCode = sqlite3_exec(db, sql.c_str(), callback, (void *)&sqlReturn, &errorStr);
        ASSERT_FALSE(errorStr) << errorStr;
        EXPECT_EQ(sqlReturn.size(), 4);
    }

    {
        std::string sql = "SELECT Id FROM Progress WHERE StageId=2 AND Finished=1;";
        SqlReturn sqlReturn;
        char *errorStr = nullptr;
        int errorCode = sqlite3_exec(db, sql.c_str(), callback, (void *)&sqlReturn, &errorStr);
        ASSERT_FALSE(errorStr) << errorStr;
        EXPECT_EQ(sqlReturn.size(), 1);
    }
}