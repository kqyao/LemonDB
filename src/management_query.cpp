#include "management_query.h"
#include "db.h"
#include "query_results.h"

#include <iostream>

using namespace std; 

constexpr const char *CopyTableQuery::qname;
constexpr const char *TruncateTableQuery::qname;
constexpr const char *LoadTableQuery::qname;
constexpr const char *DropTableQuery::qname;
constexpr const char *DumpTableQuery::qname;
constexpr const char *ListTableQuery::qname;
constexpr const char *PrintTableQuery::qname;

std::string CopyTableQuery::toString()
{
    return "QUERY = Truncate TABLE, FILE = \"" + this->newName + "\"";
}

string CopyTableQuery::commandName()
{
    return "COPYTABLE"; 
}

string CopyTableQuery::getTableName()
{
    return this->tableName; 
}

string CopyTableQuery::getTableNameSecond()
{
    return this->newName;
}

QueryResult::Ptr CopyTableQuery::execute()
{
    using namespace std;
    Database &db = Database::getInstance();
    try
    {
        //Table::Ptr table = std::make_unique<Table>(this->newTable, fields);
        //db.registerTable(std::move(table));
        db.copyTable(this->tableName, this->newName);
    }
    catch (const std::invalid_argument &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, string(e.what()));
    }
    return make_unique<SuccessMsgResult>(qname);
}

std::string TruncateTableQuery::toString()
{
    return "QUERY = Truncate TABLE, FILE = \"" + this->tableName + "\"";
}

string TruncateTableQuery::commandName()
{
    return "TRUNCATE"; 
}

string TruncateTableQuery::getTableName()
{
    return this->tableName; 
}

string TruncateTableQuery::getTableNameSecond()
{
    return ""; 
}

QueryResult::Ptr TruncateTableQuery::execute()
{
    using namespace std;
    Database &db = Database::getInstance();
    try
    {
        db.truncateTable(this->tableName);
    }
    catch (const std::invalid_argument &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, string(e.what()));
    }
    return make_unique<SuccessMsgResult>(qname);
}

std::string LoadTableQuery::toString()
{
    return "QUERY = Load TABLE, FILE = \"" + this->fileName + "\"";
}

string LoadTableQuery::commandName()
{
    return "LOAD"; 
}

string LoadTableQuery::getTableName()
{
    std::ifstream infile(this->fileName);
    string tableName;
    infile >> tableName; 
    return tableName; 
}

string LoadTableQuery::getTableNameSecond()
{
    return ""; 
}

QueryResult::Ptr LoadTableQuery::execute()
{
    std::ifstream infile(this->fileName);
    if (!infile.is_open())
    {
        return std::make_unique<ErrorMsgResult>(
            qname, "Cannot open file '?'"_f % this->fileName);
    }
    Table::Ptr table = nullptr;
    try
    {
        table = loadTableFromStream(infile, this->fileName);
        Database &db = Database::getInstance();
        db.registerTable(std::move(table));
    }
    catch (const std::exception &e)
    {
        return std::make_unique<ErrorMsgResult>(qname, e.what());
    }
    return std::make_unique<SuccessMsgResult>(qname);
}

std::string DropTableQuery::toString()
{
    return "QUERY = Drop TABLE, Table = \"" + this->tableName + "\"";
}

string DropTableQuery::commandName()
{
    return "DROP"; 
}

string DropTableQuery::getTableName()
{
    return this->tableName; 
}

string DropTableQuery::getTableNameSecond()
{
    return ""; 
}

QueryResult::Ptr DropTableQuery::execute()
{
    using namespace std;
    Database &db = Database::getInstance();
    try
    {
        db.dropTable(this->tableName);
    }
    catch (const std::invalid_argument &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, string(e.what()));
    }
    return make_unique<SuccessMsgResult>(qname);
}

std::string DumpTableQuery::toString()
{
    return "QUERY = Dump TABLE, "
           "Table = \"" +
           this->tableName + "\", "
                             "ToFile = \"" +
           this->fileName + "\"";
}

string DumpTableQuery::commandName()
{
    return "DUMP"; 
}

string DumpTableQuery::getTableName()
{
    return this->tableName; 
}

string DumpTableQuery::getTableNameSecond()
{
    return ""; 
}

QueryResult::Ptr DumpTableQuery::execute()
{
    std::ofstream ofile(this->fileName);
    if (!ofile.is_open())
    {
        return std::make_unique<ErrorMsgResult>(
            qname, tableName.c_str(),
            R"(Cannot open File "?".)"_f % this->fileName);
    }
    Database &db = Database::getInstance();
    try
    {
        ofile << (db[this->tableName]);
    }
    catch (const std::exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, tableName.c_str());
    }
    ofile.close();
    return std::make_unique<SuccessMsgResult>(
        qname, tableName.c_str());
}

QueryResult::Ptr ListTableQuery::execute()
{
    Database &db = Database::getInstance();
    db.printAllTable();
    return std::make_unique<SuccessMsgResult>(qname);
}

string ListTableQuery::commandName()
{
    return "LIST"; 
}

string ListTableQuery::getTableName()
{
    return ""; 
}

string ListTableQuery::getTableNameSecond()
{
    return ""; 
}

std::string ListTableQuery::toString()
{
    return "QUERY = LIST";
}

QueryResult::Ptr PrintTableQuery::execute()
{
    using namespace std;
    try
    {
        Database &db = Database::getInstance();
        auto &table = db[this->tableName];
        cout << "================\n";
        cout << "TABLE = ";
        cout << table;
        cout << "================\n"
             << endl;
        return make_unique<SuccessMsgResult>(
            qname, tableName.c_str());
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->tableName.c_str(),
            "No such table."s);
    }
}

string PrintTableQuery::commandName()
{
    return "SHOWTABLE";
}

string PrintTableQuery::getTableName()
{
    return this->tableName; 
}

string PrintTableQuery::getTableNameSecond()
{
    return ""; 
}

std::string PrintTableQuery::toString()
{
    return "QUERY = SHOWTABLE, Table = \"" + this->tableName + "\"";
}

QueryResult::Ptr QuitQuery::execute()
{
    return std::make_unique<NullQueryResult>();
}

string QuitQuery::commandName()
{
    return "QUIT"; 
}

string QuitQuery::getTableName()
{
    return ""; 
}

string QuitQuery::getTableNameSecond()
{
    return ""; 
}

std::string QuitQuery::toString()
{
    return "QUERY = QUIT";
}

