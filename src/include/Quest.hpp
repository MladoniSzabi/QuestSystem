#ifndef QUEST_HPP_
#define QUEST_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

/*
Schema:

CREATE TABLE Quest(
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT UNIQUE NOT NULL,
    Description TEXT
)

CREATE TABLE Quest_Requirements(
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    QuestId INTEGER NOT NULL,
    Item TEXT,
    Operand INTEGER,
    Value REAL,
    FOREIGN KEY (QuestId) REFERENCES Quest(Id)
)

CREATE TABLE Quest_Rewards(
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    QuestId INTEGER NOT NULL,
    Item TEXT,
    Value REAL,
    FOREIGN KEY (QuestId) REFERENCES Quest(Id)
)
*/

enum Operand
{
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN_OR_EQUAL
};

struct Requirement
{
    Operand operand;
    double value;

    Requirement(Operand _op, double _value)
    {
        operand = _op;
        value = _value;
    }

    Requirement()
    {
    }
};

struct Quest
{
    long id;
    std::string name;
    std::string description;
    std::unordered_map<std::string, Requirement> requirements;
    std::unordered_map<std::string, double> rewards;

    Quest()
    {
        id = 0;
    }

    Quest(long _id, std::string _name, const std::string &_description)
    {
        id = _id;
        name = _name;
        description = _description;
    }

    Quest(const std::vector<std::string> &arr)
    {
        id = std::stol(arr[0]);
        name = arr[1];
        description = arr[2];
    }

    void addRequirement(const std::vector<std::string> &arr)
    {
        requirements[arr[5]] = Requirement((Operand)std::stoi(arr[6]), std::stod(arr[7]));
    }

    bool areRequirementsMet(const std::unordered_map<std::string, double> &info)
    {
        for (const auto &pair : requirements)
        {
            if (info.find(pair.first) == info.end())
                return false;

            switch (pair.second.operand)
            {

            case EQUAL:
                if (info.at(pair.first) != pair.second.value)
                    return false;
                break;
            case NOT_EQUAL:
                if (info.at(pair.first) == pair.second.value)
                    return false;
                break;
            case LESS_THAN:
                if (info.at(pair.first) >= pair.second.value)
                    return false;
                break;
            case GREATER_THAN:
                if (info.at(pair.first) <= pair.second.value)
                    return false;
                break;
            case LESS_THAN_OR_EQUAL:
                if (info.at(pair.first) > pair.second.value)
                    return false;
                break;
            case GREATER_THAN_OR_EQUAL:
                if (info.at(pair.first) < pair.second.value)
                    return false;
                break;
            }
        }
        return true;
    }
};

#endif // QUEST_HPP_