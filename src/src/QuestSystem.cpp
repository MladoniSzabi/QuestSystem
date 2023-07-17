#include "QuestSystem.hpp"

#include <iostream>

typedef std::vector<std::vector<std::string>> SqlReturn;

int callback(void *retvalPtr, int columnCount, char **values, char **columnName)
{
    SqlReturn *retval = (SqlReturn *)retvalPtr;
    std::vector<std::string> row;
    for (int i = 0; i < columnCount; i++)
    {
        row.emplace_back(values[i]);
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
}

void QuestSystem::close()
{
    sqlite3_close_v2(_questDatabaseConn);
    _questDatabaseConn = nullptr;
}

const char *QuestSystem::startQuest(long questId)
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

const char *QuestSystem::startQuest(const std::string &questName)
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

std::vector<Quest> QuestSystem::getAllQuests()
{
    std::string sql = "SELECT * FROM Quest";
    char *errorStr = nullptr;
    SqlReturn questArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&questArray, &errorStr);
    if (errorCode)
    {
        return {};
    }

    std::vector<Quest> quests;
    for (std::vector<std::string> &quest : questArray)
    {
        quests.emplace_back(quest);
    }

    return quests;
}

std::vector<Quest> QuestSystem::getActiveQuests()
{
    std::string sql = "SELECT Quest.* FROM Quest INNER JOIN Stage ON Quest.Id=Stage.QuestId LEFT JOIN PROGRESS ON Stage.Id=Progress.StageId WHERE Stage.Level=0 AND Progress.StageId IS NOT NULL;";
    char *errorStr = nullptr;
    SqlReturn questArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&questArray, &errorStr);
    if (errorCode)
    {
        return {};
    }

    std::vector<Quest> quests;
    for (std::vector<std::string> &quest : questArray)
    {
        quests.emplace_back(quest);
    }

    return quests;
}

std::vector<Stage> QuestSystem::getActiveStages()
{
    std::string sql = "SELECT Stage.* FROM Stage LEFT JOIN PROGRESS ON Stage.Id=Progress.StageId WHERE Progress.StageId IS NOT NULL";
    char *errorStr = nullptr;
    SqlReturn stageArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&stageArray, &errorStr);
    if (errorCode)
    {
        return {};
    }

    std::vector<Stage> stages;
    for (auto &stage : stageArray)
    {
        stages.emplace_back(stage);
    }

    return stages;
}

std::vector<Stage> QuestSystem::getStagesForQuest(long questId)
{
    std::string sql = "SELECT * FROM Stage WHERE QuestId = " + std::to_string(questId);
    char *errorStr = nullptr;
    SqlReturn stagesArray;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&stagesArray, &errorStr);
    if (errorCode)
    {
        return {};
    }

    std::vector<Stage> stages;
    for (auto &stage : stagesArray)
    {
        stages.emplace_back(stage);
    }

    return stages;
}

std::vector<Quest> QuestSystem::getAvailableQuests(const std::unordered_map<std::string, double> &info)
{
    // Get the quests that the player does not have the stats for.
    std::string unavailableSql = "\
        SELECT DISTINCT Quest.Id FROM Quest \
        INNER JOIN Quest_Requirements ON Quest.Id = Quest_Requirements.QuestId \
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

    std::string finishedSql = "SELECT * FROM Quest INNER JOIN Quest_Requirements ON Quest.Id = Quest_Requirements.QuestId Where (Quest.Id NOT IN (" + unavailableSql + "))" + "AND (Quest.Id NOT IN (" + started_sql + ")) ORDER BY Id ASC";
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
    std::string sql = "SELECT * FROM Quest INNER JOIN Quest_Requirements ON Quest.Id = Quest_Requirements.QuestId Where Quest.Id = " + std::to_string(questId);
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
    std::string sql = "SELECT * FROM Stage INNER JOIN Stage_Requirements ON Stage.Id = Stage_Requirements.StageId Where Stage.Id = " + std::to_string(stageId);
    char *errorStr = nullptr;
    SqlReturn stageArr;
    int errorCode = sqlite3_exec(_questDatabaseConn, sql.c_str(), callback, (void *)&stageArr, &errorStr);
    if (errorCode)
    {
        std::cout << sql << std::endl
                  << errorStr << std::endl;
        return false;
    }

    if (stageArr.size() == 0)
    {
        std::cout << "Stage does not exist" << std::endl;
        return false;
    }

    Stage stage(stageArr[0]);
    for (const auto &line : stageArr)
    {
        stage.addRequirement(line);
    }

    return stage.areRequirementsMet(info);
}

std::vector<Stage> QuestSystem::completeStage(long stageId, const std::unordered_map<std::string, double> &info)
{
    if (!isStageCompletable(stageId, info))
    {
        return {Stage()};
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

    std::string getStageSql = "SELECT * FROM Stage Where Id=" + std::to_string(stageId);
    errorStr = nullptr;
    SqlReturn currStageArr;
    errorCode = sqlite3_exec(_questDatabaseConn, getStageSql.c_str(), callback, (void *)&currStageArr, &errorStr);
    if (errorCode)
    {
        std::cout << getStageSql << std::endl
                  << errorStr << std::endl;
        return {};
    }

    Stage currStage = Stage(currStageArr[0]);

    std::string getNextStageSql = "SELECT * FROM Stage INNER JOIN Stage_Requirements ON Stage.Id = Stage_Requirements.StageId Where Stage.QuestId=" + std::to_string(currStage.questId) + " AND Level = " + std::to_string(currStage.order + 1) + " ORDER BY Level";
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
            retval.push_back(currStage);
            currStage = Stage(stage);
        }
        currStage.addRequirement(stage);
    }
    retval.push_back(currStage);

    return retval;
}