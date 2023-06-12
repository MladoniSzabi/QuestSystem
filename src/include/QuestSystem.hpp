#ifndef QUESTSYSTEM_HPP_
#define QUESTSYSTEM_HPP_

#include <string>
#include <vector>

#include <sqlite/sqlite3.h>

#include "Quest.hpp"

class QuestSystem
{
private:
    sqlite3 *_questDatabaseConn;

public:
    QuestSystem();
    ~QuestSystem();

    bool open(std::string questDatabaseFile);
    // Used for testing
    void setDatabase(sqlite3 *connection) { _questDatabaseConn = connection; }

    char *startQuest(long questId);
    char *startQuest(std::string questName);

    std::vector<Quest> getActiveQuests();
};

#endif // QUESTSYSTEM_HPP_