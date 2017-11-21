#ifndef SRC_MANAGEMENT_QUERY_H
#define SRC_MANAGEMENT_QUERY_H

#include "query.h"

class CopyTableQuery : public Query {
    static constexpr const char* qname = "COPYTABLE";
    const std::string tableName, newName;
public:
    CopyTableQuery(std::string table, std::string newName) : tableName(table), newName(newName){}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};


class TruncateTableQuery : public Query {
    static constexpr const char* qname = "TRUNCATE";
    const std::string tableName;
public:
    TruncateTableQuery(std::string table) : tableName(table) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class LoadTableQuery : public Query {
    static constexpr const char* qname = "LOAD";
    const std::string fileName;
public:
    LoadTableQuery(std::string file) : fileName(file) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class DropTableQuery : public Query {
    static constexpr const char* qname = "DROP";
    const std::string tableName;
public:
    DropTableQuery(std::string table) : tableName(table) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class DumpTableQuery : public Query {
    static constexpr const char* qname = "DUMP";
    const std::string tableName;
    const std::string fileName;
public:
    DumpTableQuery(std::string table, std::string file)
            : tableName(table), fileName(file) {}
    QueryResult::Ptr execute() override;
    std::string      toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class ListTableQuery : public Query {
    static constexpr const char* qname = "LIST";
public:
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override;
    string getTableName() override;
    string getTableNameSecond() override; 
};

class PrintTableQuery : public Query {
    static constexpr const char* qname = "SHOWTABLE";
    const std::string tableName;
public:
    PrintTableQuery(std::string table)
            : tableName(table) {}
    QueryResult::Ptr execute()  override;
    std::string      toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class QuitQuery : public Query {
    static constexpr const char* qname = "QUIT";
public:
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override;
    string getTableName() override;
    string getTableNameSecond() override; 
};

#endif //SRC_MANAGEMENT_QUERY_H
