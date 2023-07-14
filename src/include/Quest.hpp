#ifndef QUEST_HPP_
#define QUEST_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "Requirement.hpp"

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

struct Quest
{
    long id;
    std::string name;
    std::string description;
    Requirements requirements;
    std::unordered_map<std::string, double> rewards;

    Quest() : id(0) {}
    Quest(long _id, std::string _name, const std::string &_description) : id(_id), name(_name), description(_description) {}
    Quest(const std::vector<std::string> &arr) : id(std::stol(arr[0])), name(arr[1]), description(arr[2]) {}

    void addRequirement(const std::vector<std::string> &arr) { requirements.addRequirement(arr.begin() + 5); }
    bool areRequirementsMet(const std::unordered_map<std::string, double> &info) { return requirements.areRequirementsMet(info); }
};

#endif // QUEST_HPP_