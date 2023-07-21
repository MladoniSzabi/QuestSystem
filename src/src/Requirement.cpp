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
    if (str == "=")
        return Operand::EQUAL;
    else if (str == ">")
        return Operand::GREATER_THAN;
    else if (str == ">=")
        return Operand::GREATER_THAN_OR_EQUAL;
    else if (str == "<")
        return Operand::LESS_THAN;
    else if (str == "<=")
        return Operand::LESS_THAN_OR_EQUAL;
    else if (str == "!=")
        return Operand::NOT_EQUAL;
    std::cout << "String " << str << " is not an operand." << std::endl;
    return Operand::EQUAL;
}

void Requirements::addRequirement(const std::string &item, const Requirement &requirement) { requirements[item] = requirement; }

void Requirements::addRequirement(const std::vector<std::string>::const_iterator &iterator)
{
    if (*iterator == "" || *(iterator + 1) == "" || *(iterator + 2) == "" || *(iterator + 3) == "")
    {
        return;
    }

    // td::cout << *(iterator) << " " << *(iterator + 2) << " " << *(iterator + 3) << " " << *(iterator + 4) << std::endl;
    long id = std::stol(*(iterator));
    std::string name = *(iterator + 2);
    Operand operand = (Operand)std::stoi(*(iterator + 3));
    double value = std::stod(*(iterator + 4));

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