#include <iostream>
#include <string>
#include <unordered_map>

struct Quest
{
    long id;
    std::string name;
    std::string description;
    std::unordered_map<std::string, int> requirements;
    std::unordered_map<std::string, int> rewards;
};

struct Stage
{
    long id;
    long questId;
    std::string description;
    std::unordered_map<std::string, int> requirements;
    int order;
};

struct Progress
{
    long id;
    long questId;
    long stageId;
    bool failed;
};

int main()
{
    std::cout << "Hello, World!" << std::endl;
    return 0;
}