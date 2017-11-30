#ifndef SRC_DB_H
#define SRC_DB_H

#include "db_table.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

#define __MAP__LOCK__DEBUG__

static std::mutex getInstanceLock;
//static std::mutex map_lock;

class Database
{
    static std::unique_ptr<Database> instance;
#ifdef __MAP__LOCK__DEBUG__
    std::mutex map_lock;
#endif
    std::unordered_map<std::string, std::unique_ptr<Table>> tables;

    Database() = default;
    /*Database() 
    {
        tables = std::unordered_map<std::string, std::unique_ptr<Table>>();
        tables.reserve(100); 
    }*/

  public:
    void registerTable(Table::Ptr &&table);
    void dropTable(std::string tableName);
    void truncateTable(std::string tableName);
    void copyTable(std::string tableName, std::string newName);
    void printAllTable();

    Table &operator[](std::string tableName);
    const Table &operator[](std::string tableName) const;

    Database &operator=(const Database &) = delete;
    Database &operator=(Database &&) = delete;
    Database(const Database &) = delete;
    Database(Database &&) = delete;
    ~Database() = default;

    static Database &getInstance()
    {
        if (Database::instance == nullptr)
        {
            getInstanceLock.lock(); 
            if (Database::instance == nullptr) 
            {
                instance = std::unique_ptr<Database>(new Database);
                //instance->tables.reserve(100);
                //instance = std::unique_ptr<Database>(); 
            }
            getInstanceLock.unlock(); 
        }
        return *instance;
    }
};

#endif //SRC_DB_H
