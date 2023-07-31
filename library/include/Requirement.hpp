#ifndef REQUIREMENT_HPP_
#define REQUIREMENT_HPP_

#include <set>
#include <vector>
#include <string>
#include <unordered_map>

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
    std::string name;
    Operand operand;
    double value;

    Requirement(long _id, std::string _name, Operand _op, double _value) : id(_id), name(_name), operand(_op), value(_value) {}
    Requirement() {}
};

extern std::string operandToString(Operand op);
extern Operand stringToOperand(const std::string &str);

struct Requirements
{
    std::vector<Requirement> requirements;
    void addRequirement(const Requirement &requirement);
    void addRequirement(const std::vector<std::string>::const_iterator &iterator);
    bool areRequirementsMet(const std::unordered_map<std::string, double> &info);
};

#endif // REQUIREMENT_HPP_