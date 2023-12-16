#ifndef LOICOLLECTION_SQLITEDATABASE_H
#define LOICOLLECTION_SQLITEDATABASE_H

#include <string>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

class SQLiteDatabase {
private:
    SQLite::Database db;
    std::string table = "data";

public:
    SQLiteDatabase(const std::string& db_name);
    void createTable();
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void remove(const std::string& key);
    void update(const std::string& key, const std::string& value);
    void setTable(const std::string& tableName);
    std::vector<std::string> list();
    std::vector<std::string> listTabe(const std::string& tableName);
    bool exists(const std::string& key);
    bool existsTable(const std::string& tableName);
    void removeTable(const std::string& tableName);
    void close();
};

#endif