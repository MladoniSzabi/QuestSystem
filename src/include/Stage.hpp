#ifndef STAGE_HPP_
#define STAGE_HPP_

#include <string>
#include <vector>

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
};

#endif // STAGE_HPP_