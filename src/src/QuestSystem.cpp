#include "QuestSystem.hpp"

typedef std::vector<std::vector<std::string>> SqlReturn;

static int callback(void *, int, char **, char **)
{
    return 0;
}

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

bool QuestSystem::startQuest(long questId)
{
    std::string sql = "INSERT INTO Progress(QuestID, RequirementID, Finished) VALUES (" + std::to_string(questId) + ", 0, 0)";
    char *errorStr;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, nullptr, &errorStr);
    return errorCode;
}

bool QuestSystem::startQuest(std::string questName)
{
    std::string getQuestIdSql = "SELECT ID FROM QUEST WHERE Name=" + questName;
    char *errorStr;
    SqlReturn sqlReturn;
    int errorCode = sqlite3_exec(_questDatabaseConn, getQuestIdSql.c_str(), callback, (void *)&sqlReturn, &errorStr);

    if (sqlReturn.size() == 0)
    {
        return false;
    }

    long questId = std::stol(sqlReturn[0][0]);
    return startQuest(questId);
}

std::vector<Quest> QuestSystem::getActiveQuests()
{
    std::string sql = "SELECT * FROM Quest LEFT JOIN PROGRESS ON Quest.ID=Progress.QuestID WHERE Progress.QuestID IS NULL";
    char *errorStr;
    SqlReturn questArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&questArray, &errorStr);
    if (errorCode)
    {
        return {};
    }

    std::vector<Quest> quests;
    for (std::vector<std::string> &quest : questArray)
    {
        quests.push_back(Quest(quest));
    }

    return quests;
}