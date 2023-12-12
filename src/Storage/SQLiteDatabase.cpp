#include "SQLiteDatabase.h"

SQLiteDatabase::SQLiteDatabase(const std::string& db_name) : db(db_name, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE) {
    createTable();
}

void SQLiteDatabase::createTable() {
    db.exec("CREATE TABLE IF NOT EXISTS data (key TEXT PRIMARY KEY, value TEXT)");
}

void SQLiteDatabase::set(const std::string& key, const std::string& value) {
    SQLite::Statement query(db, "INSERT OR REPLACE INTO data VALUES (?, ?)");
    query.bind(1, key);
    query.bind(2, value);
    query.exec();
}

std::string SQLiteDatabase::get(const std::string& key) {
    SQLite::Statement query(db, "SELECT value FROM data WHERE key = ?");
    query.bind(1, key);
    if (query.executeStep()) {
        return query.getColumn(0).getText();
    }
    return nullptr;
}

void SQLiteDatabase::remove(const std::string& key) {
    SQLite::Statement query(db, "DELETE FROM data WHERE key = ?");
    query.bind(1, key);
    query.exec();
}

void SQLiteDatabase::update(const std::string& key, const std::string& value) {
    SQLite::Statement query(db, "UPDATE data SET value = ? WHERE key = ?");
    query.bind(1, value);
    query.bind(2, key);
    query.exec();
}

bool SQLiteDatabase::exists(const std::string& key) {
    SQLite::Statement query(db, "SELECT * FROM data WHERE key = ?");
    query.bind(1, key);
    return query.executeStep();
}

void SQLiteDatabase::close() {
    db.~Database();
}