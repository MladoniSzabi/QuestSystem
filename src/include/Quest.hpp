#ifndef QUEST_HPP_
#define QUEST_HPP_

#include <string>
#include <vector>

struct Quest
{
    long id;
    std::string name;
    std::string description;

    Quest(std::vector<std::string> &arr)
    {
        id = std::stol(arr[0]);
        name = arr[1];
        description = arr[2];
    }
};

#endif // QUEST_HPP_