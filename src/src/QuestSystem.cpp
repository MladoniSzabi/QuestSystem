#include "QuestSystem.hpp"

#include <iostream>

typedef std::vector<std::vector<std::string>> SqlReturn;

int callback(void *retvalPtr, int columnCount, char **values, char **columnName)
{
    SqlReturn *retval = (SqlReturn *)retvalPtr;
    std::vector<std::string> row;
    for (int i = 0; i < columnCount; i++)
    {
        if (values[i])
        {
            row.emplace_back(values[i]);
        }
        else
        {
            row.push_back("");
        }
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

bool QuestSystem::open(const std::string &questDatabaseFile)
{
    int error = sqlite3_open(questDatabaseFile.c_str(), &_questDatabaseConn);
    if (error)
    {
        return false;
    }
    return true;

    getActiveStages();
}

void QuestSystem::close()
{
    sqlite3_close_v2(_questDatabaseConn);
    _questDatabaseConn = nullptr;
}

const char *QuestSystem::startQuest(long questId)
{
    std::string select = "SELECT * FROM Stage LEFT JOIN Stage_Requirements ON Stage.Id=Stage_Requirements.StageId WHERE QuestId=" + std::to_string(questId) + " AND Level=0;";
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

    Stage currStage = Stage(sqlReturn[0]);
    for (auto &stage : sqlReturn)
    {
        currStage.addRequirement(stage);
    }
    _activeStagesCache[currStage.id] = currStage;

    std::string sql = "INSERT INTO Progress(StageId, Finished) VALUES (" + sqlReturn[0][0] + ", 0)";
    errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), nullptr, nullptr, &errorStr);
    return errorStr;
}

const char *QuestSystem::startQuest(const std::string &questName)
{
    std::string getQuestIdSql = "SELECT Stage.*, Stage_Requirements.* FROM Stage LEFT JOIN Stage_Requirements ON Stage.Id=Stage_Requirements.StageId INNER JOIN Quest ON Stage.QuestId=Quest.Id WHERE Quest.Name='" + questName + "' AND Stage.Level=0;";
    char *errorStr = nullptr;
    SqlReturn sqlReturn;
    int errorCode = sqlite3_exec(_questDatabaseConn, getQuestIdSql.c_str(), callback, (void *)&sqlReturn, &errorStr);
    if (sqlReturn.size() == 0)
    {
        return "Quest not found";
    }

    Stage currStage = Stage(sqlReturn[0]);
    for (auto &stage : sqlReturn)
    {
        currStage.addRequirement(stage);
    }
    _activeStagesCache[currStage.id] = currStage;

    std::string sql = "INSERT INTO Progress(StageId, Finished) VALUES (" + sqlReturn[0][0] + ", 0)";
    errorStr = nullptr;
    errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), nullptr, nullptr, &errorStr);
    return errorStr;
}

std::vector<Quest> QuestSystem::getAllQuests()
{
    std::string sql = "SELECT * FROM Quest LEFT JOIN Quest_Requirements ON Quest.Id=Quest_Requirements.QuestId ORDER BY Quest.Id";
    char *errorStr = nullptr;
    SqlReturn questArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&questArray, &errorStr);
    if (errorCode)
    {
        std::cout << sql << std::endl
                  << errorStr << std::endl;
        return {};
    }
    if (questArray.size() == 0)
    {
        return {};
    }

    std::vector<Quest> quests;
    Quest currQuest(questArray[0]);
    for (std::vector<std::string> &quest : questArray)
    {
        if (currQuest.id != std::stol(quest[0]))
        {
            quests.push_back(currQuest);
            currQuest = Quest(quest);
        }
        currQuest.addRequirement(quest);
    }
    quests.push_back(currQuest);

    return quests;
}

std::vector<Quest> QuestSystem::getActiveQuests()
{
    std::string sql = "SELECT Quest.*, Quest_Requirements.* FROM Quest INNER JOIN Stage ON Quest.Id=Stage.QuestId LEFT JOIN PROGRESS ON Stage.Id=Progress.StageId LEFT JOIN Quest_Requirements ON Quest.Id=Quest_Requirements.QuestId WHERE Stage.Level=0 AND Progress.StageId IS NOT NULL ORDER BY Quest.Id;";
    char *errorStr = nullptr;
    SqlReturn questArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&questArray, &errorStr);
    if (errorCode)
    {
        std::cout << sql << std::endl
                  << errorStr << std::endl;
        return {};
    }

    if (questArray.size() == 0)
    {
        return {};
    }

    std::vector<Quest> quests;
    Quest currQuest(questArray[0]);
    for (std::vector<std::string> &quest : questArray)
    {
        if (currQuest.id != std::stol(quest[0]))
        {
            quests.push_back(currQuest);
            currQuest = Quest(quest);
        }
        currQuest.addRequirement(quest);
    }
    quests.push_back(currQuest);

    return quests;
}

std::vector<Stage> QuestSystem::getActiveStages()
{
    std::string sql = "SELECT Stage.*, Stage_Requirements.* FROM Stage LEFT JOIN PROGRESS ON Stage.Id=Progress.StageId  LEFT JOIN Stage_Requirements ON Stage.Id=Stage_Requirements.StageId WHERE Progress.StageId IS NOT NULL ORDER BY Stage.Id";
    char *errorStr = nullptr;
    SqlReturn stageArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&stageArray, &errorStr);
    if (errorCode)
    {
        std::cout << sql << std::endl
                  << errorStr << std::endl;
        return {};
    }

    if (stageArray.size() == 0)
    {
        return {};
    }

    _activeStagesCache.clear();
    std::vector<Stage> retval;
    Stage currStage = Stage(stageArray[0]);
    for (auto &stage : stageArray)
    {
        if (currStage.id != std::stol(stage[0]))
        {
            _activeStagesCache[currStage.id] = currStage;
            retval.push_back(currStage);
            currStage = Stage(stage);
        }
        currStage.addRequirement(stage);
    }
    _activeStagesCache[currStage.id] = currStage;
    retval.push_back(currStage);

    return retval;
}

std::vector<Stage> QuestSystem::getStagesForQuest(long questId)
{
    std::string sql = "SELECT * FROM Stage LEFT JOIN Stage_Requirements ON Stage.Id=Stage_Requirements.StageId WHERE QuestId = " + std::to_string(questId);
    char *errorStr = nullptr;
    SqlReturn stagesArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&stagesArray, &errorStr);
    if (errorCode)
    {
        std::cout << sql << std::endl
                  << errorStr << std::endl;
        return {};
    }

    if (stagesArray.size() == 0)
    {
        return {};
    }

    std::vector<Stage> stages;
    Stage currStage = Stage(stagesArray[0]);
    for (auto &stage : stagesArray)
    {
        if (currStage.id != std::stol(stage[0]))
        {
            stages.push_back(currStage);
            currStage = Stage(stage);
        }
        currStage.addRequirement(stage);
    }
    stages.push_back(currStage);

    return stages;
}

std::vector<Quest> QuestSystem::getAvailableQuests(const std::unordered_map<std::string, double> &info)
{
    // Get the quests that the player does not have the stats for.
    std::string unavailableSql = "\
        SELECT DISTINCT Quest.Id FROM Quest \
        LEFT JOIN Quest_Requirements ON Quest.Id = Quest_Requirements.QuestId \
        WHERE ( \
                Quest_Requirements.Operand = '>' OR \
                Quest_Requirements.Operand = '>=' OR \
                Quest_Requirements.Operand = '=' \
        )";

    if (!info.empty())
    {
        unavailableSql += " AND (";
        for (auto &it : info)
        {
            unavailableSql += "(Quest_Requirements.Item = '" + it.first + "' AND Quest_Requirements.Value < " + std::to_string(it.second) + ") OR ";
        }
        unavailableSql += "0)";
    }

    // Omit quests that have already been started.
    std::string started_sql = "SELECT DISTINCT Quest.Id FROM Quest INNER JOIN Stage ON Quest.Id=Stage.QuestId LEFT JOIN PROGRESS ON Stage.Id=Progress.StageId WHERE Stage.Level=0 AND Progress.StageId IS NOT NULL";

    std::string finishedSql = "SELECT * FROM Quest LEFT JOIN Quest_Requirements ON Quest.Id = Quest_Requirements.QuestId Where (Quest.Id NOT IN (" + unavailableSql + "))" + "AND (Quest.Id NOT IN (" + started_sql + ")) ORDER BY Id ASC";
    char *errorStr = nullptr;
    SqlReturn quests;
    int errorCode = sqlite3_exec(_questDatabaseConn, finishedSql.c_str(), callback, (void *)&quests, &errorStr);
    if (errorCode)
    {
        std::cout << finishedSql << std::endl
                  << errorStr << std::endl;
        return {};
    }

    std::vector<Quest> retval;
    Quest currQuest;
    for (auto &quest : quests)
    {
        if (currQuest.id != std::stol(quest[0]))
        {
            if (currQuest.id != 0 && currQuest.areRequirementsMet(info))
            {
                retval.push_back(currQuest);
            }
            currQuest = Quest(quest);
        }
        currQuest.addRequirement(quest);
    }

    if (currQuest.id != 0 && currQuest.areRequirementsMet(info))
    {
        retval.push_back(currQuest);
    }

    return retval;
}

bool QuestSystem::isQuestAvailable(long questId, const std::unordered_map<std::string, double> &info)
{
    std::string sql = "SELECT * FROM Quest LEFT JOIN Quest_Requirements ON Quest.Id = Quest_Requirements.QuestId Where Quest.Id = " + std::to_string(questId);
    char *errorStr = nullptr;
    SqlReturn questArr;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&questArr, &errorStr);
    if (errorCode)
    {
        std::cout << sql << std::endl
                  << errorStr << std::endl;
        return {};
    }

    Quest quest(questArr[0]);
    for (const auto &line : questArr)
    {
        quest.addRequirement(line);
    }

    return quest.areRequirementsMet(info);
}

bool QuestSystem::isStageCompletable(long stageId, const std::unordered_map<std::string, double> &info)
{
    if (_activeStagesCache.find(stageId) == _activeStagesCache.end())
    {
        return false;
    }

    return _activeStagesCache[stageId].areRequirementsMet(info);
}

std::vector<Stage> QuestSystem::completeStage(long stageId, const std::unordered_map<std::string, double> &info)
{
    if (!isStageCompletable(stageId, info))
    {
        return {Stage()};
    }

    Stage currStage = _activeStagesCache[stageId];

    // TODO: rephrase this:
    // Active stages with the same quest id are different ways to complete a quest
    // Since one of them is done the other paths are not valid anymore so remove them all
    for (auto i = _activeStagesCache.begin(), last = _activeStagesCache.end(); i != last;)
    {
        if (i->second.questId == currStage.questId)
        {
            i = _activeStagesCache.erase(i);
        }
        else
        {
            ++i;
        }
    }

    std::string completeStageSql = "INSERT INTO Progress(StageId, Finished) VALUES (" + std::to_string(stageId) + ", 1)";
    char *errorStr = nullptr;
    int errorCode = sqlite3_exec(_questDatabaseConn, completeStageSql.c_str(), nullptr, nullptr, &errorStr);
    if (errorCode)
    {
        std::cout << completeStageSql << std::endl
                  << errorStr << std::endl;
        return {};
    }

    std::string getNextStageSql = "SELECT * FROM Stage LEFT JOIN Stage_Requirements ON Stage.Id = Stage_Requirements.StageId Where Stage.QuestId=" + std::to_string(currStage.questId) + " AND Level = " + std::to_string(currStage.order + 1) + " ORDER BY Level";
    errorStr = nullptr;
    SqlReturn stageArr;
    errorCode = sqlite3_exec(_questDatabaseConn, getNextStageSql.c_str(), callback, (void *)&stageArr, &errorStr);
    if (errorCode)
    {
        std::cout << getNextStageSql << std::endl
                  << errorStr << std::endl;
        return {};
    }

    if (stageArr.size() == 0)
    {
        return {};
    }

    std::vector<Stage> retval;
    currStage = Stage(stageArr[0]);
    for (auto &stage : stageArr)
    {
        if (currStage.id != std::stol(stage[0]))
        {
            _activeStagesCache[currStage.id] = currStage;
            retval.push_back(currStage);
            currStage = Stage(stage);
        }
        currStage.addRequirement(stage);
    }
    _activeStagesCache[currStage.id] = currStage;
    retval.push_back(currStage);

    return retval;
}