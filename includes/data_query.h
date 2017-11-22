#ifndef SRC_DATA_QUERY_H
#define SRC_DATA_QUERY_H

#include "query.h"

using namespace std; 

class ComplexQuery : public ConditionedQuery
{
protected:
    std::string targetTable;
    std::vector<std::string> operands;
    std::vector<QueryCondition> condition;

public:
    ComplexQuery(const std::string &targetTable,
                             const std::vector<std::string> &operands,
                             const std::vector<QueryCondition> &condition)
            : targetTable(targetTable),
                operands(operands),
                condition(condition) {}
};

// Here follows the basic CURD Queries:
// C - Create (insert) an entry
// U - Update (existing query)
// R - Read   (read existsing queries, implemented as dump)
// D - Delete (delete existing query)

class UpdateQuery : public ComplexQuery
{
    static constexpr const char *qname = "UPDATE";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class DeleteQuery : public ComplexQuery
{
    static constexpr const char *qname = "DELETE";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class InsertQuery : public ComplexQuery
{
    static constexpr const char *qname = "INSERT";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class SelectQuery : public ComplexQuery
{
    static constexpr const char *qname = "SELECT";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class DuplicateQuery : public ComplexQuery
{
    static constexpr const char *qname = "DUPLICATE";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class SwapQuery : public ComplexQuery
{
    static constexpr const char *qname = "SWAP";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class CountQuery : public ComplexQuery
{
    static constexpr const char *qname = "COUNT";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override;
};

class AddQuery : public ComplexQuery
{
    static constexpr const char *qname = "ADD";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class SubQuery : public ComplexQuery
{
    static constexpr const char *qname = "SUB";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class SumQuery : public ComplexQuery
{
    static constexpr const char *qname = "SUM";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class MinQuery : public ComplexQuery
{
    static constexpr const char *qname = "MIN";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

class MaxQuery : public ComplexQuery
{
    static constexpr const char *qname = "MAX";

public:
    using ComplexQuery::ComplexQuery;
    QueryResult::Ptr execute() override;
    std::string toString() override;
    string commandName() override; 
    string getTableName() override;
    string getTableNameSecond() override; 
};

#endif //SRC_DATA_QUERY_H
