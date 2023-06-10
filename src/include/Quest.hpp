#ifndef QUEST_HPP_
#define QUEST_HPP_

#include <string>
#include <vector>

struct Quest
{
    long id;
    std::string name;
    std::string description;
    std::string reward; // In JSON format

    Quest(std::vector<std::string> &arr)
    {
        id = std::stol(arr[0]);
        name = arr[1];
        description = arr[2];
        reward = arr[3];
    }
};

#endif // QUEST_HPP_