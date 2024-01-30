#pragma once

#ifdef LOICOLLECTION_SQLITEDATABASE_API
#else
#define LOICOLLECTION_SQLITEDATABASE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

#ifdef __cplusplus
extern "C" {
#endif
class LOICOLLECTION_SQLITEDATABASE_API SQLiteDatabase {
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
    std::vector<std::string> listTable(const std::string& tableName);
    bool exists(const std::string& key);
    bool existsTable(const std::string& tableName);
    void removeTable(const std::string& tableName);
    void createMap(const std::string& tableName);
    std::string getMap(const std::string& tableName, const std::string& key);
    void dropMap(const std::string& tableName, const std::string& key);
    void setMap(const std::string& tableName, const std::string& key, const std::string& value);
    void updateMap(const std::string& tableName, const std::string& key, const std::string& value);
    std::vector<std::string> listMap(const std::string& tableName);
    bool existsMap(const std::string& tableName);
    void removeMap(const std::string& tableName);
    void close();
};
#ifdef __cplusplus
}
#endif