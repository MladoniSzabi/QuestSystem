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
};

#endif // QUESTSYSTEM_HPP_