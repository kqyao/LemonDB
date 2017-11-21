#ifndef SRC_QUERY_H
#define SRC_QUERY_H

#include <string>
#include <memory>
#include <thread>
#include <iostream>
#include <mutex>
#include "uexception.h"
#include "db_table.h"
#include "query_results.h"

using namespace std; 

/************** things added*************/

typedef enum lastStatus_t {
    STATUS_WRITE = 0x00, 
    STATUS_READ = 0x01
} lastStatus; 

static unordered_map<string, vector<vector<thread*>>> table_thread_group; 
// map string(tablename) to thread groups; 
// vector<thread*> is a group of reading threads or writing threads; 
//                 we push `reading` threads into one vector
//                 `writing` threads into different vectors
// vector<vector<thread*>> is the whole list of these groups; 

static unordered_map<string, lastStatus> table_last_status; 
// save the last status of each table. 

static unordered_map<thread*, mutex*> thread_join_lock; 
// the hash table of lock when test joinable. 

static std::vector<mutex*> mutex_vector;
// contain all mutexes need to be delete (free). 

////////////////////////////////////////////////////

class Query {
public:
    typedef std::unique_ptr<Query> Ptr;
    virtual QueryResult::Ptr execute() = 0;
    virtual std::string      toString() = 0;
    virtual ~Query() = default;

    /******added ***************/
    //********************** DANGEROUS PUBLIC ATTRIBUTES!!! We are being LAZY here!!! ************
    vector<thread*>* waitThreadsPtr = nullptr; // wait the threads in the vector before execute
    vector<thread*>* waitThreadsSecondPtr = nullptr; // only for COPYTABLE
    virtual string commandName() = 0; // return things like "LOAD" or "SELECT", etc. 
    virtual string getTableName() = 0;  // return the table needed or affected. 
    virtual string getTableNameSecond() = 0; // only for COPYTABLE
    void executeAndPrint() 
    {
        if (waitThreadsPtr) {
            for (auto it=waitThreadsPtr->begin(); it!=waitThreadsPtr->end(); ++it) 
            {
                mutex* lockPtr = thread_join_lock[*it];
                lockPtr->lock(); 
                if ((*it)->joinable())
                    (*it)->join(); 
                lockPtr->unlock(); 
            }
        }
        if (waitThreadsSecondPtr) {
            for (auto it=waitThreadsSecondPtr->begin(); it!=waitThreadsSecondPtr->end(); ++it) 
            {
                mutex* lockPtr = thread_join_lock[*it];
                lockPtr->lock(); 
                if ((*it)->joinable())
                    (*it)->join(); 
                lockPtr->unlock(); 
            }
        }
        QueryResult::Ptr result = execute();
        if (result->success())
        {
            cout << result->toString();
        }
        else
        {
            std::flush(cout);
            cerr << "QUERY FAILED:\n\t" << result->toString();
        }
    }
    //////////////////////////////////////////////
};

struct QueryCondition {
    std::string field;
    std::string op;
    std::string value;
};

class NopQuery : public Query {
public:
    QueryResult::Ptr execute() override {
        return std::make_unique<NullQueryResult>();
    }

    std::string toString() override {
        return "QUERY = NOOP";
    }
    string commandName() override {
        return "NOOP"; 
    }

    string getTableName() override {
        return ""; 
    }
    string getTableNameSecond() override {
        return ""; 
    }
};


class ConditionedQuery : public Query {
protected:
    bool evalCondition(const std::vector<QueryCondition>& conditions,
                       const Table::Object& object);
};



#endif //SRC_QUERY_H
