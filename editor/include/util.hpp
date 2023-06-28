#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <string>

#include "sqlite/sqlite3.h"

extern bool isNumber(const std::string &s);
extern void updateSql(sqlite3 *db, const std::string &table, const std::string &field, const std::string &value, long id);

#endif // UTIL_HPP_