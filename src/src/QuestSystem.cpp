#include "QuestSystem.hpp"

typedef std::vector<std::vector<std::string>> SqlReturn;

int callback(void *retvalPtr, int columnCount, char **values, char **columnName)
{
    SqlReturn *retval = (SqlReturn *)retvalPtr;
    std::vector<std::string> row;
    for (int i = 0; i < columnCount; i++)
    {
        row.push_back(std::string(values[i]));
    }
    retval->push_back(row);
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

char *QuestSystem::startQuest(long questId)
{
    std::string sql = "INSERT INTO Progress(QuestID, RequirementID, Finished) VALUES (" + std::to_string(questId) + ", 0, 0)";
    char *errorStr = nullptr;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), nullptr, nullptr, &errorStr);
    return errorStr;
}

char *QuestSystem::startQuest(std::string questName)
{
    std::string getQuestIdSql = "SELECT Id FROM Quest WHERE Name='" + questName + "';";
    char *errorStr = nullptr;
    SqlReturn sqlReturn;
    int errorCode = sqlite3_exec(_questDatabaseConn, getQuestIdSql.c_str(), callback, (void *)&sqlReturn, &errorStr);
    if (sqlReturn.size() == 0)
    {
        return "Quest not found";
    }

    long questId = std::stol(sqlReturn[0][0]);
    return startQuest(questId);
}

std::vector<Quest> QuestSystem::getActiveQuests()
{
    std::string sql = "SELECT * FROM Quest LEFT JOIN PROGRESS ON Quest.ID=Progress.QuestID WHERE Progress.QuestID IS NOT NULL";
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