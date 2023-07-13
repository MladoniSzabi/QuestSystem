#ifndef STAGE_HPP_
#define STAGE_HPP_

#include <string>
#include <vector>

/*
Schema:

CREATE TABLE Stage(
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    QuestId INTEGER NOT NULL,
    Description TEXT,
    Level INTEGER,
    FOREIGN KEY (QuestId) REFERENCES Quest(Id)
)
*/

struct Stage
{
    long id;
    long questId;
    std::string description;
    long order;

    Stage(std::vector<std::string> fields)
    {
        id = std::stol(fields[0]);
        questId = std::stol(fields[1]);
        description = fields[2];
        order = std::stol(fields[3]);
    }

    Stage(long _id, long _questId, std::string _description, long _order)
    {
        id = _id;
        questId = _questId;
        description = _description;
        order = _order;
    }
};

#endif // STAGE_HPP_