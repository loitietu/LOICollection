#define LOICOLLECTION_SQLITEDATABASE_API __declspec(dllexport)

#include <string>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>
#include "SQLiteDatabase.h"

SQLiteDatabase::SQLiteDatabase(const std::string& db_name) : db(db_name, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE) {
    createTable();
}

void SQLiteDatabase::createTable() {
    db.exec("CREATE TABLE IF NOT EXISTS " + table + " (key TEXT PRIMARY KEY, value TEXT);");
}

void SQLiteDatabase::set(const std::string& key, const std::string& value) {
    SQLite::Statement query(db, "INSERT OR REPLACE INTO " + table + " VALUES (?, ?);");
    query.bind(1, key);
    query.bind(2, value);
    query.exec();
}

std::string SQLiteDatabase::get(const std::string& key) {
    SQLite::Statement query(db, "SELECT value FROM " + table + " WHERE key = ?;");
    query.bind(1, key);
    if (query.executeStep()) {
        return query.getColumn(0).getText();
    }
    return nullptr;
}

void SQLiteDatabase::remove(const std::string& key) {
    SQLite::Statement query(db, "DELETE FROM " + table + " WHERE key = ?;");
    query.bind(1, key);
    query.exec();
}

void SQLiteDatabase::update(const std::string& key, const std::string& value) {
    SQLite::Statement query(db, "UPDATE " + table + " SET value = ? WHERE key = ?;");
    query.bind(1, value);
    query.bind(2, key);
    query.exec();
}

void SQLiteDatabase::setTable(const std::string& tableName) {
    table = tableName;
}

std::vector<std::string> SQLiteDatabase::list() {
    std::vector<std::string> listName;
    SQLite::Statement query(db, "SELECT name FROM sqlite_master WHERE type='table';");
    while (query.executeStep()) {
        listName.push_back(query.getColumn(0).getString());
    }
    return listName;
}

std::vector<std::string> SQLiteDatabase::listTable(const std::string& tableName) {
    std::vector<std::string> listName;
    SQLite::Statement query(db, "SELECT * FROM " + tableName + ";");
    while (query.executeStep()) {
        listName.push_back(query.getColumn(0).getString());
    }
    return listName;
}

bool SQLiteDatabase::exists(const std::string& key) {
    SQLite::Statement query(db, "SELECT * FROM " + table + " WHERE key = ?;");
    query.bind(1, key);
    return query.executeStep();
}

bool SQLiteDatabase::existsTable(const std::string& tableName) {
    SQLite::Statement query(db, "SELECT name FROM sqlite_master WHERE type='table' AND name = ?;");
    query.bind(1, tableName);
    return query.executeStep();
}

void SQLiteDatabase::removeTable(const std::string& tableName) {
    SQLite::Statement query(db, "DROP TABLE IF EXISTS " + tableName + ";");
    query.exec();
}

void SQLiteDatabase::createMap(const std::string& tableName) {
    this->set(tableName, "Map");
    db.exec("CREATE TABLE IF NOT EXISTS " + table + "_" + tableName + " (key TEXT PRIMARY KEY, value TEXT);");
}

std::string SQLiteDatabase::getMap(const std::string& tableName, const std::string& key) {
    SQLite::Statement query(db, "SELECT value FROM " + table + "_" + tableName + " WHERE key = ?;");
    query.bind(1, key);
    if (query.executeStep()) {
        return query.getColumn(0).getText();
    }
    return nullptr;
}

void SQLiteDatabase::dropMap(const std::string& tableName, const std::string& key) {
    SQLite::Statement query(db, "DELETE FROM " + table + "_" + tableName + " WHERE key = ?;");
    query.bind(1, key);
    query.exec();
}

void SQLiteDatabase::setMap(const std::string& tableName, const std::string& key, const std::string& value) {
    SQLite::Statement query(db, "INSERT OR REPLACE INTO " + table + "_" + tableName + " VALUES (?, ?);");
    query.bind(1, key);
    query.bind(2, value);
    query.exec();
}

void SQLiteDatabase::updateMap(const std::string& tableName, const std::string& key, const std::string& value) {
    SQLite::Statement query(db, "UPDATE " + table + "_" + tableName + " SET value = ? WHERE key = ?;");
    query.bind(1, value);
    query.bind(2, key);
    query.exec();
}

std::vector<std::string> SQLiteDatabase::listMap(const std::string& tableName) {
    std::vector<std::string> listName;
    SQLite::Statement query(db, "SELECT * FROM " + table + "_" + tableName + ";");
    while (query.executeStep()) {
        listName.push_back(query.getColumn(0).getString());
    }
    return listName;
}

bool SQLiteDatabase::existsMap(const std::string& tableName) {
    SQLite::Statement query(db, "SELECT name FROM sqlite_master WHERE type='table' AND name = ?;");
    query.bind(1, table + "_" +tableName);
    return query.executeStep();
}

void SQLiteDatabase::removeMap(const std::string& tableName) {
    this->remove(tableName);
    SQLite::Statement query(db, "DROP TABLE IF EXISTS " + table + "_" + tableName + ";");
    query.exec();
}

void SQLiteDatabase::close() {
    db.~Database();
}