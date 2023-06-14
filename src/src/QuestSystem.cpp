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
    close();
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

void QuestSystem::close()
{
    sqlite3_close_v2(_questDatabaseConn);
    _questDatabaseConn = nullptr;
}

char *QuestSystem::startQuest(long questId)
{
    std::string select = "SELECT Id FROM Stage WHERE QuestId=" + std::to_string(questId) + " AND Level=0;";
    SqlReturn sqlReturn;
    char *errorStr = nullptr;
    int errorCode = sqlite3_exec(_questDatabaseConn, select.c_str(), callback, (void *)&sqlReturn, &errorStr);
    if (errorStr)
    {
        return errorStr;
    }
    if (sqlReturn.size() == 0)
    {
        return "Quest not found";
    }

    std::string sql = "INSERT INTO Progress(StageId, Finished) VALUES (" + sqlReturn[0][0] + ", 0)";
    errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), nullptr, nullptr, &errorStr);
    return errorStr;
}

char *QuestSystem::startQuest(std::string questName)
{
    std::string getQuestIdSql = "SELECT Stage.Id FROM Stage INNER JOIN Quest ON Stage.QuestId=Quest.Id WHERE Quest.Name='" + questName + "' AND Stage.Level=0;";
    char *errorStr = nullptr;
    SqlReturn sqlReturn;
    int errorCode = sqlite3_exec(_questDatabaseConn, getQuestIdSql.c_str(), callback, (void *)&sqlReturn, &errorStr);
    if (sqlReturn.size() == 0)
    {
        return "Quest not found";
    }
    std::string sql = "INSERT INTO Progress(StageId, Finished) VALUES (" + sqlReturn[0][0] + ", 0)";
    errorStr = nullptr;
    errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), nullptr, nullptr, &errorStr);
    return errorStr;
}

std::vector<Quest> QuestSystem::getActiveQuests()
{
    std::string sql = "SELECT Quest.* FROM Quest INNER JOIN Stage ON Quest.Id=Stage.QuestId LEFT JOIN PROGRESS ON Stage.Id=Progress.StageId WHERE Stage.Level=0 AND Progress.StageId IS NOT NULL;";
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

std::vector<Stage> QuestSystem::getActiveStages()
{
    std::string sql = "SELECT Stage.* FROM Stage LEFT JOIN PROGRESS ON Stage.Id=Progress.StageId WHERE Progress.StageId IS NOT NULL";
    char *errorStr;
    SqlReturn questArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&questArray, &errorStr);
    if (errorCode)
    {
        return {};
    }

    std::vector<Stage> stages;
    for (std::vector<std::string> &stage : questArray)
    {
        stages.push_back(Stage(stage));
    }

    return stages;
}