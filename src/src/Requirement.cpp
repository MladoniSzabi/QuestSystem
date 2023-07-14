#include "Requirement.hpp"

void Requirements::addRequirement(const std::string &item, const Requirement &requirement) { requirements[item] = requirement; }

void Requirements::addRequirement(const std::vector<std::string>::const_iterator &iterator)
{
    std::string name = *iterator;
    Operand operand = (Operand)std::stoi(*(iterator + 1));
    double value = std::stod(*(iterator + 2));

    requirements[name] = Requirement(operand, value);
}

bool Requirements::areRequirementsMet(const std::unordered_map<std::string, double> &info)
{
    for (const auto &pair : requirements)
    {
        if (info.find(pair.first) == info.end())
            return false;

        switch (pair.second.operand)
        {

        case Operand::EQUAL:
            if (info.at(pair.first) != pair.second.value)
                return false;
            break;
        case Operand::NOT_EQUAL:
            if (info.at(pair.first) == pair.second.value)
                return false;
            break;
        case Operand::LESS_THAN:
            if (info.at(pair.first) >= pair.second.value)
                return false;
            break;
        case Operand::GREATER_THAN:
            if (info.at(pair.first) <= pair.second.value)
                return false;
            break;
        case Operand::LESS_THAN_OR_EQUAL:
            if (info.at(pair.first) > pair.second.value)
                return false;
            break;
        case Operand::GREATER_THAN_OR_EQUAL:
            if (info.at(pair.first) < pair.second.value)
                return false;
            break;
        }
    }
    return true;
}