#ifndef QUEST_HPP_
#define QUEST_HPP_

#include <string>
#include <vector>

struct Quest
{
    long id;
    std::string name;
    std::string description;

    Quest(long _id, std::string _name, std::string _description)
    {
        id = _id;
        name = _name;
        _description = description;
    }

    Quest(std::vector<std::string> &arr)
    {
        id = std::stol(arr[0]);
        name = arr[1];
        description = arr[2];
    }
};

#endif // QUEST_HPP_