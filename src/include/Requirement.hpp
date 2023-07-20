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
    long id;
    Operand operand;
    double value;

    Requirement(long _id, Operand _op, double _value) : id(_id), operand(_op), value(_value) {}
    Requirement() {}
};

extern std::string operandToString(Operand op);
extern Operand stringToOperand(const std::string &str);

struct Requirements
{
    std::unordered_map<std::string, Requirement> requirements;
    void addRequirement(const std::string &item, const Requirement &requirement);
    void addRequirement(const std::vector<std::string>::const_iterator &iterator);
    bool areRequirementsMet(const std::unordered_map<std::string, double> &info);
};

#endif // REQUIREMENT_HPP_