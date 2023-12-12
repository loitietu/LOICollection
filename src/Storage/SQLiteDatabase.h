#ifndef LOICOLLECTION_SQLITEDATABASE_H
#define LOICOLLECTION_SQLITEDATABASE_H

#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

class SQLiteDatabase {
private:
    SQLite::Database db;

public:
    SQLiteDatabase(const std::string& db_name);
    void createTable();
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void remove(const std::string& key);
    void update(const std::string& key, const std::string& value);
    bool exists(const std::string& key);
    void close();
};

#endif