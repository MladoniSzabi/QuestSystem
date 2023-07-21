#include "util.hpp"

#include <algorithm>
#include <iostream>

bool isNumber(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c)
                                      { return !std::isdigit(c); }) == s.end();
}

void updateSql(sqlite3 *db, const std::string &table, const std::string &field, const std::string &value, long id)
{
    char *errmsg = nullptr;
    std::string escapedValue = value;
    size_t pos = escapedValue.find("\"", pos);
    while (pos != std::string::npos)
    {
        escapedValue.replace(pos, 1, "\\\"");
        pos = escapedValue.find("\"", pos + 1);
    }
    std::string sql = "UPDATE " + table + " SET " + field + " = \"" + escapedValue + "\" WHERE Id=" + std::to_string(id);
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc)
    {
        std::cout << "Error updating " << field << ": " << errmsg << " | " << sql << std::endl;
    }
}

int createEntryCallback(void *ptr, int rowCount, char **values, char **rowNames)
{
    long *newRow = (long *)newRow;
    *newRow = atol(values[0]);
    return 0;
}