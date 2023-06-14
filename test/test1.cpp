#include <gtest/gtest.h>
#include <sqlite/sqlite3.h>
#include "QuestSystem.hpp"

class QuestSystemFixture : public ::testing::Test
{
private:
    sqlite3 *db;

protected:
    QuestSystem qs;
    void SetUp() override
    {

        char *errmsg;

        // Open sharable inmemory database
        int rc = sqlite3_open(":memory:", &db);
        ASSERT_EQ(rc, 0) << sqlite3_errmsg(db);

        // Create Quest Database
        sqlite3_exec(
            db,
            "\
                CREATE TABLE Quest(Id INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT UNIQUE NOT NULL, Description TEXT);\
                CREATE TABLE Stage(Id INTEGER PRIMARY KEY AUTOINCREMENT, QuestId INTEGER NOT NULL, Description TEXT,\
                    DependsOn INTEGER, FOREIGN KEY (QuestId) REFERENCES Quest(Id), FOREIGN KEY (DependsOn) REFERENCES Stage(Id));\
                CREATE TABLE Progress(Id INTEGER PRIMARY KEY AUTOINCREMENT, StageId INTEGER NOT NULL, Finished INTEGER,\
                    FOREIGN KEY (StageId) REFERENCES Stage(Id));\
                INSERT INTO Quest(Name, Description) VALUES ('Hello, World!', 'This is the first quest you will go on!'), ('Goodbye, Wold!', 'Test');\
                INSERT INTO Stage(QuestId, Description, DependsOn) VALUES (1, 'Step 1', NULL), (1, 'Step 2', 1), (2, 'Test1', NULL);\
            ",
            nullptr,
            nullptr,
            &errmsg);

        // Pass the database to the quest system
        qs.setDatabase(db);
    }

    void TearDown() override
    {
        qs.close();
    }
};

TEST_F(QuestSystemFixture, TestNoActive)
{
    EXPECT_EQ(qs.getActiveQuests().size(), 0);
}

TEST_F(QuestSystemFixture, TestStartQuest)
{
    EXPECT_EQ(qs.getActiveQuests().size(), 0);
    char *error = qs.startQuest(1);
    EXPECT_EQ(error, nullptr) << error;
    EXPECT_EQ(qs.getActiveQuests().size(), 1);
    error = qs.startQuest("Goodbye, Wold!");
    EXPECT_EQ(error, nullptr) << error;
    EXPECT_EQ(qs.getActiveQuests().size(), 2);
}