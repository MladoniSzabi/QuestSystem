#include "Requirement.hpp"

#include <iostream>

std::string operandToString(Operand op)
{
    switch (op)
    {
    case Operand::EQUAL:
        return "=";
    case Operand::GREATER_THAN:
        return ">";
    case Operand::GREATER_THAN_OR_EQUAL:
        return ">=";
    case Operand::LESS_THAN:
        return "<";
    case Operand::LESS_THAN_OR_EQUAL:
        return "<=";
    case Operand::NOT_EQUAL:
        return "!=";
    }
}

Operand stringToOperand(const std::string &str)
{
    switch (str)
    {
    case "=":
        return Operand::EQUAL;
    case ">":
        return Operand::GREATER_THAN;
    case ">=":
        return Operand::GREATER_THAN_OR_EQUAL;
    case "<":
        return Operand::LESS_THAN;
    case "<=":
        return Operand::LESS_THAN_OR_EQUAL;
    case "!=":
        return Operand::NOT_EQUAL;
    default:
        std::cout << "String " << str << " is not an operand." << std::endl;
        return Operand::EQUAL;
    }
}

void Requirements::addRequirement(const std::string &item, const Requirement &requirement) { requirements[item] = requirement; }

void Requirements::addRequirement(const std::vector<std::string>::const_iterator &iterator)
{
    if (*iterator == "" || *(iterator + 1) == "" || *(iterator + 2) == "" || *(iterator + 3) == "")
    {
        return;
    }

    long id = std::stol(*(iterator));
    std::string name = *(iterator + 1);
    Operand operand = (Operand)std::stoi(*(iterator + 2));
    double value = std::stod(*(iterator + 3));

    requirements[name] = Requirement(id, operand, value);
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