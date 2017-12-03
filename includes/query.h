#ifndef SRC_QUERY_H
#define SRC_QUERY_H

#include <string>
#include <memory>
#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <list>
#include <condition_variable>
#include "uexception.h"
#include "db_table.h"
#include "query_results.h"

using namespace std;

/************** things added*************/

typedef enum lastStatus_t {
    STATUS_WRITE = 0x00,
    STATUS_READ = 0x01
} lastStatus;

extern unordered_map<string, vector<vector<thread*>>> table_thread_group;
// map string(tablename) to thread groups; 
// vector<thread*> is a group of reading threads or writing threads; 
//                 we push `reading` threads into one vector
//                 `writing` threads into different vectors
// vector<vector<thread*>> is the whole list of these groups; 

extern unordered_map<string, lastStatus> table_last_status;
// save the last status of each table.

// this type is used on table column prev stage remember
// it can be used for the parallel in a subset of write operation
typedef enum last_col_status_t {
    UNUSED = 0x00,
    READ = 0x01,
    WROTE = 0x02
} last_col_status;

// this global var is used to store all table col prev stage
extern unordered_map<string, unordered_map<string, last_col_status>> tables_cols_last_status;

extern unordered_map<thread*, mutex*> thread_join_lock;
// the hash table of lock when test joinable. 

extern std::vector<mutex*> mutex_vector;
// contain all mutexes that need to be delete (free). 

extern std::vector<thread*> thread_vector;
// contain all threads that need to be delete (free). 

//extern std::vector<string> output_vector;
extern list<string> output_list;
// contain all the output in order. 

//static mutex cout_lock; ////////
static mutex thread_join_lock_lock; //////////
// extern mutex output_vector_lock; 
extern int counter;
extern mutex counter_lock;
extern std::condition_variable cv;
extern mutex mtx;
////////////////////////////////////////////////////

class Query {
public:
    typedef std::unique_ptr<Query> Ptr;
    virtual QueryResult::Ptr execute() = 0;
    virtual std::string      toString() = 0;
    virtual ~Query() = default;

    /******added ***************/
    //********************** DANGEROUS PUBLIC ATTRIBUTES!!! We are being LAZY here!!! ************
    //int queryID; // this is the No. queryID query. 
    string* outputString; // store the query output
    vector<thread*> waitThreadsPtr = vector<thread*>(); // wait the threads in the vector before execute
    vector<thread*> waitThreadsSecondPtr = vector<thread*>(); // only for COPYTABLE
    vector<thread*> waitThreadsOutputPtr = vector<thread*>(); // wait the threads before output
    virtual string commandName() = 0; // return things like "LOAD" or "SELECT", etc. 
    virtual string getTableName() = 0;  // return the table needed or affected. 
    virtual string getTableNameSecond() = 0; // only for COPYTABLE
    void executeAndPrint()
    {
        string tableN = getTableName();
        //cerr << "h0: " << commandName() << endl; ////////////////
        if (!waitThreadsPtr.empty()) {
            for (auto it=waitThreadsPtr.begin(); it!=waitThreadsPtr.end(); ++it)
            {
                thread_join_lock_lock.lock(); ////////////
                mutex* lockPtr = thread_join_lock[*it];
                thread_join_lock_lock.unlock(); ///////////////
                //cout_lock.lock();//////////
                //cerr << commandName() << " " << queryID << " " << lockPtr << endl;  ///////////
                //cout_lock.unlock();/////////////
                lockPtr->lock();
                if ((*it)->joinable()) {
                    (*it)->join();
                    //cerr << "join!!!!!" << endl;
                }else {
                    /*
                    counter_lock.lock();//////
                    --counter;
                    cerr << counter << "-----" << endl;
                    counter_lock.unlock();/////
                     */
                    //(*it)->detach();
                }
                lockPtr->unlock();
            }
        }
        if (!waitThreadsSecondPtr.empty()) {
            for (auto it=waitThreadsSecondPtr.begin(); it!=waitThreadsSecondPtr.end(); ++it)
            {
                mutex* lockPtr = thread_join_lock[*it];
                lockPtr->lock();
                if ((*it)->joinable()) {
                    (*it)->join();
                    //cerr << "join!!!!!" << endl;
                }else {
                    /*
                    counter_lock.lock();//////
                    --counter;
                    cerr << counter << "-----" << endl;
                    counter_lock.unlock();/////
                     */
                    //(*it)->detach();
                }
                lockPtr->unlock();
            }
        }
        //cerr << "h1: " << commandName() << endl; ////////////////
        QueryResult::Ptr result = execute();
        if (!(*result).success())
        {
            std::flush(cout);
            cerr << "QUERY FAILED:\n\t" << (*result).toString();
        }
        //else{cout << result->toString();}
        //cerr << "finished\n";
        counter_lock.lock();//////
        --counter;
        //cerr << counter << "-----" << endl;
        counter_lock.unlock();/////
        //cv.notify_all();
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
