#ifndef QUESTSYSTEM_HPP_
#define QUESTSYSTEM_HPP_

#include <string>
#include <vector>
#include <unordered_map>

#include <sqlite/sqlite3.h>

#include "Quest.hpp"
#include "Stage.hpp"

class QuestSystem
{
private:
    sqlite3 *_questDatabaseConn = nullptr;
    std::unordered_map<long, Stage> _activeStagesCache;

public:
    QuestSystem();
    ~QuestSystem();

    bool open(const std::string &questDatabaseFile);
    void close();
    // Used for testing.
    void setDatabase(sqlite3 *connection) { _questDatabaseConn = connection; }

    const char *startQuest(long questId);
    const char *startQuest(const std::string &questName);

    std::vector<Quest> getAllQuests();

    std::vector<Quest> getActiveQuests();
    std::vector<Stage> getActiveStages();

    std::vector<Stage> getStagesForQuest(long questId);

    // Returns the quests that the player can start with its current inventory/stats.
    std::vector<Quest> getAvailableQuests(const std::unordered_map<std::string, double> &info);
    bool isQuestAvailable(long questId, const std::unordered_map<std::string, double> &info);

    bool isStageCompletable(long stageId, const std::unordered_map<std::string, double> &info);
    std::vector<Stage> completeStage(long stageId, const std::unordered_map<std::string, double> &info); // Returns the next stage(s)
};

#endif // QUESTSYSTEM_HPP_