#include "QuestSystem.hpp"

QuestSystem::QuestSystem()
{
}

QuestSystem::~QuestSystem()
{
}

bool QuestSystem::open(std::string questDatabaseFile)
{
    int error = sqlite3_open(questDatabaseFile.c_str(), &_questDatabaseConn);
    if (error)
    {
        return false;
    }
    return true;
}