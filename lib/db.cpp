#include <iostream>
#include "db.h"
#include "uexception.h"

std::unique_ptr<Database> Database::instance = nullptr;

void Database::registerTable(Table::Ptr &&table)
{
    auto name = table->name();
#ifdef __MAP__LOCK__DEBUG__
    map_lock.lock();
#endif
    if (this->tables.find(name) != this->tables.end())
        throw DuplicatedTableName(
            "Error when inserting table \"" + name + "\". Name already exists.");
    this->tables[name] = std::move(table);
#ifdef __MAP__LOCK__DEBUG__
    map_lock.unlock();
#endif
}

Table &Database::operator[](std::string tableName)
{
#ifdef __MAP__LOCK__DEBUG__
    map_lock.lock();
#endif
    auto it = this->tables.find(tableName);
    if (it == this->tables.end())
        throw TableNameNotFound(
            "Error accesing table \"" + tableName + "\". Table not found.");
    auto& return_value = *(it->second);
#ifdef __MAP__LOCK__DEBUG__
    map_lock.unlock();
#endif
    return return_value;
}

const Table &Database::operator[](std::string tableName) const
{
    auto it = this->tables.find(tableName);
    if (it == this->tables.end())
        throw TableNameNotFound(
            "Error accesing table \"" + tableName + "\". Table not found.");
    auto& return_value = *(it->second);
    return return_value;
}

void Database::dropTable(std::string name)
{
#ifdef __MAP__LOCK__DEBUG__
    map_lock.lock();
#endif
    auto it = this->tables.find(name);
    if (it == this->tables.end())
        throw TableNameNotFound(
            "Error when trying to drop table \"" + name + "\". Table not found.");
    this->tables.erase(it);
#ifdef __MAP__LOCK__DEBUG__
    map_lock.unlock();
#endif
}

void Database::truncateTable(std::string name)
{
#ifdef __MAP__LOCK__DEBUG__
    map_lock.lock();
#endif
    auto it = this->tables.find(name);
    if (it == this->tables.end())
        throw TableNameNotFound(
            "Error when trying to truncate table \"" + name + "\". Table not found.");
#ifdef __MAP__LOCK__DEBUG__
    map_lock.unlock();
#endif

    Database &db = Database::getInstance();
    auto &table = db[name];
    for (Table::Iterator it_tb = table.begin(); it_tb != table.end(); it_tb = table.begin())
        table.erase(it_tb);
}

void Database::copyTable(std::string name, std::string newName)
{
#ifdef __MAP__LOCK__DEBUG__
    map_lock.lock();
#endif
    auto it = this->tables.find(name);
    if (it == this->tables.end())
        throw TableNameNotFound(
            "Error when trying to copy table \"" + name + "\". Table not found.");
#ifdef __MAP__LOCK__DEBUG__
    map_lock.unlock();
#endif

    Database &db = Database::getInstance();
    auto &table = db[name];
    //std::vector<std::string> tmp_field(table.field());
    Table::Ptr newTable = std::make_unique<Table>(newName, table.field());
    db.registerTable(std::move(newTable));
    auto &dest = db[newName];
    for (auto object : table)
    {
        std::vector<int> tuple;
        for (int i = 0; i < table.field().size(); i++)
            tuple.push_back(object[i]);
        dest.insertByIndex(object.key(), tuple);
    }
    /*
    for (Table::Iterator it_tb = table.begin(); it_tb != table.end(); ++it_tb)
    {
        std::vector<int> tuple;
        //std::cout << it_tb->key() << std::endl;
        for (int i=0;i<table.field().size();i++)
            tuple.push_back(it_tb->get(table.field()[i]));
        dest.insertByIndex(it_tb->key(), tuple);
    }
    */
}

void Database::printAllTable()
{
    std::cout << "Database overview:" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << std::setw(15) << "Table name";
    std::cout << std::setw(15) << "# of fields";
    std::cout << std::setw(15) << "# of entries" << std::endl;
    for (const auto &table : this->tables)
    {
        std::cout << std::setw(15) << table.first;
        std::cout << std::setw(15) << (*table.second).field().size() + 1;
        std::cout << std::setw(15) << (*table.second).size() << std::endl;
    }
    std::cout << "Total " << this->tables.size() << " tables." << std::endl;
    std::cout << "=========================" << std::endl;
}
