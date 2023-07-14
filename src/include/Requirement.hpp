#ifndef REQUIREMENT_HPP_
#define REQUIREMENT_HPP_

#include <unordered_map>
#include <vector>
#include <string>

enum class Operand
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

    Requirement(Operand _op, double _value) : operand(_op), value(_value) {}
    Requirement() {}
};

class Requirements
{
private:
    std::unordered_map<std::string, Requirement> requirements;

public:
    void addRequirement(const std::string &item, const Requirement &requirement);
    void addRequirement(const std::vector<std::string>::const_iterator &iterator);
    bool areRequirementsMet(const std::unordered_map<std::string, double> &info);
};

#endif // REQUIREMENT_HPP_