#include "db.h"
#include "data_query.h"
#include <iostream>
#include <algorithm>
#include <sstream>
constexpr const char *InsertQuery::qname;
constexpr const char *UpdateQuery::qname;
constexpr const char *DeleteQuery::qname;
constexpr const char *SelectQuery::qname;
constexpr const char *DuplicateQuery::qname;
constexpr const char *SwapQuery::qname;
constexpr const char *CountQuery::qname;
constexpr const char *AddQuery::qname;
constexpr const char *SubQuery::qname;
constexpr const char *SumQuery::qname;
constexpr const char *MinQuery::qname;
constexpr const char *MaxQuery::qname;

using namespace std; 

QueryResult::Ptr SubQuery::execute()
{
    using namespace std;
    if (this->operands.size() < 2)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        int counter = 0;
        string dest = operands[operands.size() - 1];
        string src = operands[0];
        for (auto object : table)
        {
            if (evalCondition(condition, object))
            {
                int temp = object[src];
                for (int i = 1; i < operands.size() - 1; temp -= object[operands[i]], i++)
                    ;
                object[dest] = temp;
                counter++;
            }
        }
        //output_vector[this->queryID] = "Affected ? rows.\n"_f % counter;
        *outputString = "Affected ? rows.\n"_f % counter;
        return make_unique<RecordCountResult>(counter);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string SubQuery::commandName()
{
    return "SUB";
}

string SubQuery::getTableName()
{
    return this->targetTable; 
}

string SubQuery::getTableNameSecond()
{
    return ""; 
}

std::string SubQuery::toString()
{
    return "QUERY = SUB " + this->targetTable + "\"";
}

QueryResult::Ptr AddQuery::execute()
{
    using namespace std;
    if (this->operands.size() < 2)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        int counter = 0;
        string dest = operands[operands.size() - 1];
        for (auto object : table)
        {
            if (evalCondition(condition, object))
            {
                int destValue = 0; 
                for (int i = 0; i < operands.size() - 1; destValue += object[operands[i]], i++)
                    ;
                object[dest] = destValue; 
                counter++;
            }
        }
        //output_vector[this->queryID] = "Affected ? rows.\n"_f % counter;
        *outputString = "Affected ? rows.\n"_f % counter;
        return make_unique<RecordCountResult>(counter);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string AddQuery::commandName()
{
    return "ADD"; 
}

string AddQuery::getTableName()
{
    return this->targetTable; 
}

string AddQuery::getTableNameSecond()
{
    return ""; 
}

std::string AddQuery::toString()
{
    return "QUERY = ADD " + this->targetTable + "\"";
}

QueryResult::Ptr SumQuery::execute()
{
    using namespace std;
    if (this->operands.size() == 0)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        unordered_map<int, int> output;
        //int len = table.field().size();
        for (int i=0; i<operands.size(); i++) 
        {
            output[i]=0; 
        }
        for (auto object : table)
        {
            if (evalCondition(condition, object))
                for (int i = 0; i < operands.size(); output[i] += object[operands[i]], i++)
                    ;
        }
        ostringstream outputStream; 
        outputStream << "ANSWER = ( ";
        for (int i=0; i<operands.size(); i++) 
        {
            outputStream << output[i] << " "; 
        }
        outputStream << ")\n";
        //output_vector[this->queryID] = outputStream.str(); 
        *outputString = outputStream.str(); 
        return make_unique<SuccessMsgResult>(qname);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string SumQuery::commandName()
{
    return "SUM"; 
}

string SumQuery::getTableName()
{
    return this->targetTable; 
}

string SumQuery::getTableNameSecond()
{
    return ""; 
}

std::string SumQuery::toString()
{
    return "QUERY = SUM " + this->targetTable + "\"";
}

QueryResult::Ptr MaxQuery::execute()
{
    using namespace std;
    if (this->operands.size() == 0)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        map<string, vector<int>> output;
        int len = table.field().size();
        for (auto object : table)
        {
            if (evalCondition(condition, object))
                for (int i = 0; i < operands.size(); output[operands[i]].push_back(object[operands[i]]), i++)
                    ;
        }
        if (!output.empty()) 
        {
            ostringstream outputStream; 
            outputStream << "ANSWER = ( ";
            for (int i=0; i<operands.size(); i++) 
            {
                outputStream << *max_element(output[operands[i]].begin(), output[operands[i]].end()) << " "; 
            }
            outputStream << ")\n";
            //output_vector[this->queryID] = outputStream.str(); 
            *outputString = outputStream.str(); 
        }
        return make_unique<SuccessMsgResult>(qname);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string MaxQuery::commandName() 
{
    return "MAX"; 
}

string MaxQuery::getTableName()
{
    return this->targetTable; 
}

string MaxQuery::getTableNameSecond()
{
    return ""; 
}

std::string MaxQuery::toString()
{
    return "QUERY = MAX " + this->targetTable + "\"";
}

QueryResult::Ptr MinQuery::execute()
{
    using namespace std;
    if (this->operands.size() == 0)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        map<string, vector<int>> output;
        int len = table.field().size();
        for (auto object : table)
        {
            if (evalCondition(condition, object))
                for (int i = 0; i < operands.size(); output[operands[i]].push_back(object[operands[i]]), i++)
                    ;
        }
        if(!output.empty()) 
        {
            ostringstream outputStream; 
            outputStream << "ANSWER = ( ";
            for (int i=0; i<operands.size(); i++) 
            {
                outputStream << *min_element(output[operands[i]].begin(), output[operands[i]].end()) << " "; 
            }
            outputStream << ")\n";
            //output_vector[this->queryID] = outputStream.str(); 
            *outputString = outputStream.str(); 
        }
        return make_unique<SuccessMsgResult>(qname);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string MinQuery::commandName()
{
    return "MIN"; 
}

string MinQuery::getTableName()
{
    return this->targetTable; 
}

string MinQuery::getTableNameSecond()
{
    return ""; 
}

std::string MinQuery::toString()
{
    return "QUERY = MIN " + this->targetTable + "\"";
}

QueryResult::Ptr SwapQuery::execute()
{
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        int counter = 0, temp;
        for (auto object : table)
        {
            if (evalCondition(condition, object))
            {
                counter++;
                temp = object[operands[0]];
                object[operands[0]] = object[operands[1]];
                object[operands[1]] = temp;
            }
        }
        //output_vector[this->queryID] = "Affected ? rows.\n"_f % counter;
        *outputString = "Affected ? rows.\n"_f % counter; 
        return make_unique<RecordCountResult>(counter);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string SwapQuery::commandName()
{
    return "SWAP"; 
}

string SwapQuery::getTableName()
{
    return this->targetTable; 
}

string SwapQuery::getTableNameSecond()
{
    return ""; 
}

std::string SwapQuery::toString()
{
    return "QUERY = SWAP " + this->targetTable + "\"";
}

QueryResult::Ptr CountQuery::execute()
{
    using namespace std;
    if (this->operands.size() != 0)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        int counter = 0;
        for (auto object : table)
        {
            if (evalCondition(condition, object))
                counter++;
        }
        //output_vector[this->queryID] = "ANSWER = ?\n"_f % counter;
        *outputString = "ANSWER = ?\n"_f % counter;
        return make_unique<SuccessMsgResult>(qname);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string CountQuery::commandName()
{
    return "COUNT"; 
}

string CountQuery::getTableName()
{
    return this->targetTable; 
}

string CountQuery::getTableNameSecond()
{
    return ""; 
}

std::string CountQuery::toString()
{
    return "QUERY = COUNT " + this->targetTable + "\"";
}

QueryResult::Ptr DuplicateQuery::execute()
{
    using namespace std;
    if (this->operands.size() != 0)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        unordered_map<string, vector<int>> output;
        int len = table.field().size();
        int counter = 0;
        for (auto object : table)
        {
            if (evalCondition(condition, object))
                for (int i = 0; i < len; output[object.key()].push_back(object[i]), i++)
                    ;
        }
        unordered_map<string, vector<int>>::iterator it = output.begin();
        while (it != output.end())
        {
            if (table.ifKeyExists((*it).first + "_copy")) {
                ++it; 
                continue; 
            }
            table.insertByIndex((*it).first + "_copy", (*it).second);
            ++it;
            counter++;
        }
        //output_vector[this->queryID] = "Affected ? rows.\n"_f % counter;
        *outputString = "Affected ? rows.\n"_f % counter;
        return make_unique<RecordCountResult>(counter);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string DuplicateQuery::commandName()
{
    return "DUPLICATE"; 
}

string DuplicateQuery::getTableName()
{
    return this->targetTable; 
}

string DuplicateQuery::getTableNameSecond()
{
    return ""; 
}

std::string DuplicateQuery::toString()
{
    return "QUERY = DUPLICATE " + this->targetTable + "\"";
}

QueryResult::Ptr SelectQuery::execute()
{
    using namespace std;
    if (this->operands.size() == 0)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    try
    {
        auto &table = db[this->targetTable];
        map<string, vector<int>> output;
        //int len = table.field().size();
        for (auto object : table)
        {
            if (evalCondition(condition, object)) {
                output[object.key()] = vector<int>(); 
                for (int i = 1; i < operands.size(); output[object.key()].push_back(object[operands[i]]), i++);
                //use "i=1" to omit KEY
            }
        }
        map<string, vector<int>>::iterator it = output.begin();
        if (it!=output.end()) 
        {
            ostringstream outputStream; 
            while (it != output.end())
            {
                outputStream << "( ? "_f % (*it).first;
                for (int i = 0; i < (*it).second.size(); i++)
                    outputStream << (*it).second[i] << " ";
                outputStream << ")\n";
                ++it;
            }
            //output_vector[this->queryID] = outputStream.str(); 
            *outputString = outputStream.str(); 
        }
        return make_unique<SuccessMsgResult>(qname);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string SelectQuery::commandName()
{
    return "SELECT"; 
}

string SelectQuery::getTableName()
{
    return this->targetTable; 
}

string SelectQuery::getTableNameSecond()
{
    return ""; 
}

std::string SelectQuery::toString()
{
    return "QUERY = SELECT " + this->targetTable + "\"";
}

QueryResult::Ptr DeleteQuery::execute()
{
    using namespace std;
    if (this->operands.size() != 0)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    int counter = 0;
    try
    {
        auto &table = db[this->targetTable];
        Table::Iterator it_tb = table.begin();
        while (it_tb != table.end())
        {
            if (evalCondition(condition, *it_tb))
            {
                it_tb = table.erase(it_tb);
                counter++;
            }
            else
                ++it_tb;
        }
        //output_vector[this->queryID] = "Affected ? rows.\n"_f % counter;
        *outputString = "Affected ? rows.\n"_f % counter;
        return make_unique<RecordCountResult>(counter);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string DeleteQuery::commandName()
{
    return "DELETE"; 
}

string DeleteQuery::getTableName()
{
    return this->targetTable; 
}

string DeleteQuery::getTableNameSecond()
{
    return ""; 
}

std::string DeleteQuery::toString()
{
    return "QUERY = DELETE " + this->targetTable + "\"";
}

QueryResult::Ptr InsertQuery::execute()
{
    using namespace std;
    Database &db = Database::getInstance();
    string key = operands[0];
    try
    {
        auto &table = db[this->targetTable];
        if (this->operands.size() != table.field().size() + 1)
            return make_unique<ErrorMsgResult>(
                qname, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size());
        std::vector<int> tuple;
        for (int i = 1; i <= table.field().size(); i++)
            tuple.push_back(stoi(operands[i]));
        table.insertByIndex(operands[0], tuple);
        return make_unique<SuccessMsgResult>(qname);
    }
    catch (const ConflictingKey &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
}

string InsertQuery::commandName()
{
    return "INSERT"; 
}

string InsertQuery::getTableName()
{
    return this->targetTable; 
}

string InsertQuery::getTableNameSecond()
{
    return ""; 
}

std::string InsertQuery::toString()
{
    return "QUERY = INSERT " + this->targetTable + "\"";
}

QueryResult::Ptr UpdateQuery::execute()
{
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    string field = operands[0];
    int counter = 0;
    try
    {
        int newValue = std::stoi(operands[1]);
        auto &table = db[this->targetTable];
        for (auto object : table)
        {
            if (evalCondition(condition, object))
            {
                object[field] = newValue;
                counter++;
            }
        }
        //output_vector[this->queryID] = "Affected ? rows.\n"_f % counter;
        *outputString = "Affected ? rows.\n"_f % counter;
        return make_unique<RecordCountResult>(counter);
    }
    catch (const TableNameNotFound &e)
    {
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "No such table."s);
    }
    catch (const TableFieldNotFound &e)
    {
        // Field not found
        return make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const IllFormedQueryCondition &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            e.what());
    }
    catch (const invalid_argument &e)
    {
        // Cannot convert operand to string
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unknown error '?'"_f % e.what());
    }
    catch (const exception &e)
    {
        return std::make_unique<ErrorMsgResult>(
            qname, this->targetTable.c_str(),
            "Unkonwn error '?'."_f % e.what());
    }
}

string UpdateQuery::commandName()
{
    return "UPDATE"; 
}

string UpdateQuery::getTableName()
{
    return this->targetTable; 
}

string UpdateQuery::getTableNameSecond()
{
    return ""; 
}

std::string UpdateQuery::toString()
{
    return "QUERY = UPDATE " + this->targetTable + "\"";
}
