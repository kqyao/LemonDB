#include <iostream>

#include "db_table.h"
#include "query_parser.h"
#include "query_builders.h"
#include "query.h"
#include "db.h"
#include <thread>
#include <vector>
#include <map>
//#include <unistd.h> ///////////////////

using namespace std;

int counter;
mutex counter_lock;
//std::condition_variable cv;
mutex mtx;
// following global variables defined externally in "query.h"
unordered_map<string, vector<vector<thread *>>> table_thread_group;
unordered_map<string, lastStatus> table_last_status;
unordered_map<string, unordered_map<string, last_col_status>> tables_cols_last_status;
unordered_map<thread *, mutex *> thread_join_lock;
std::vector<mutex *> mutex_vector;
std::vector<thread *> thread_vector;
//std::vector<string> output_vector;
list<string> output_list;

std::string extractQueryString() {
    std::string buf;
    do {
        int ch = cin.get();
        if (ch == ';')
            return buf;
        if (ch == EOF)
            throw std::ios_base::failure("End of input");
        buf.push_back(ch);
    } while (1);
}

int mult_thread_calc() {
    int val = std::thread::hardware_concurrency();
    if (val > 0) return val * 3;
    else return 10;
}

int main() {
    counter = 0;
    QueryParser p;
    //p.registerQueryBuilder(std::make_unique<FakeQueryBuilder>());
    //p.registerQueryBuilder(std::make_unique<QueryBuilder(UpdateTable)>());
    //p.registerQueryBuilder(std::make_unique<QueryBuilder(Insert)>());
    //p.registerQueryBuilder(std::make_unique<QueryBuilder(Delete)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageData)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Debug)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageTable)>());
    //int queryIDCounter = 0;
    const int mult_thread_num = mult_thread_calc();
    while (cin) {
        //std::flush(cout);
        try {
            //cerr << queryIDCounter << endl; ///////////////
            //sleep(1); //////////
            // A very standard REPL
            // REPL: Read-Evaluate-Print-Loop
            std::string queryStr = extractQueryString();
            Query::Ptr query = p.parseQuery(queryStr);
            string commandName = (*query).commandName();
            if (commandName == "QUIT") {

                break;
            }
            //query->queryID = queryIDCounter; 
            //output_vector.push_back(string());
            output_list.push_back(string());
            (*query).outputString = &(*(--output_list.end()));
            string tableName = (*query).getTableName();
            while(1) {
                if (counter >= mult_thread_num) {
                    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    std::this_thread::yield();
                    //std::unique_lock<std::mutex> lck(mtx);
                    //cv.wait(lck);
                } else break;
            }


            if (commandName == "COPYTABLE") {
                string tableNameSecond = (*query).getTableNameSecond();
                if (!table_thread_group[tableNameSecond].empty()) {
                    (*query).waitThreadsSecondPtr = *(--table_thread_group[tableNameSecond].end());
                }
                if (table_last_status[tableName] == STATUS_WRITE) {
                    (*query).waitThreadsPtr = *(--table_thread_group[tableName].end());
                    counter_lock.lock();//////
                    ++counter;
                    //cerr << counter << endl;
                    counter_lock.unlock();/////
                    thread *currentThread = new thread(&Query::executeAndPrint, move(query));
                    thread_vector.push_back(currentThread);
                    std::vector<thread *> currentThreadVector;
                    currentThreadVector.push_back(currentThread);
                    table_thread_group[tableName].push_back(currentThreadVector);
                    std::vector<thread *> currentThreadVectorSecond;
                    currentThreadVectorSecond.push_back(currentThread);
                    table_thread_group[tableNameSecond].push_back(currentThreadVectorSecond);
                    mutex *currentThreadMutex = new mutex;
                    mutex_vector.push_back(currentThreadMutex);
                    thread_join_lock_lock.lock();/////////////
                    thread_join_lock[currentThread] = currentThreadMutex;
                    thread_join_lock_lock.unlock();/////////////
                    table_last_status[tableName] = STATUS_READ;
                    table_last_status[tableNameSecond] = STATUS_WRITE;
                } else {
                    (*query).waitThreadsPtr = *(--(--table_thread_group[tableName].end()));
                    (*query).waitThreadsOutputPtr = *(--table_thread_group[tableName].end());
                    counter_lock.lock();//////
                    ++counter;
                    //cerr << counter << endl;
                    counter_lock.unlock();/////
                    thread *currentThread = new thread(&Query::executeAndPrint, move(query));
                    thread_vector.push_back(currentThread);
                    (--table_thread_group[tableName].end())->push_back(currentThread);
                    std::vector<thread *> currentThreadVectorSecond;
                    currentThreadVectorSecond.push_back(currentThread);
                    table_thread_group[tableNameSecond].push_back(currentThreadVectorSecond);
                    mutex *currentThreadMutex = new mutex;
                    mutex_vector.push_back(currentThreadMutex);
                    thread_join_lock_lock.lock();/////////////
                    thread_join_lock[currentThread] = currentThreadMutex;
                    thread_join_lock_lock.unlock();/////////////
                    table_last_status[tableName] = STATUS_READ;
                    table_last_status[tableNameSecond] = STATUS_WRITE;
                }
            } else if (commandName == "LOAD" || commandName == "DROP" || commandName == "TRUNCATE"
                       || commandName == "DELETE" || commandName == "INSERT" || commandName == "UPDATE"
                       || commandName == "DUPLICATE" || commandName == "SWAP" || commandName == "ADD" ||
                       commandName == "SUB") {
                /* Try to make changes here */
                if (!table_thread_group[tableName].empty()) {
                    (*query).waitThreadsPtr = *(--table_thread_group[tableName].end());
                }
                counter_lock.lock();//////
                ++counter;
                //cerr << counter << endl;
                counter_lock.unlock();/////
                thread *currentThread = new thread(&Query::executeAndPrint, move(query));
                thread_vector.push_back(currentThread);
                std::vector<thread *> currentThreadVector;
                currentThreadVector.push_back(currentThread);
                table_thread_group[tableName].push_back(currentThreadVector);
                mutex *currentThreadMutex = new mutex;
                mutex_vector.push_back(currentThreadMutex);
                thread_join_lock_lock.lock();/////////////
                thread_join_lock[currentThread] = currentThreadMutex;
                thread_join_lock_lock.unlock();/////////////
                table_last_status[tableName] = STATUS_WRITE;
            } else {
                if (table_last_status[tableName] == STATUS_WRITE) {
                    (*query).waitThreadsPtr = *(--table_thread_group[tableName].end());
                    counter_lock.lock();//////
                    ++counter;
                    //cerr << counter << endl;
                    counter_lock.unlock();/////
                    thread *currentThread = new thread(&Query::executeAndPrint, move(query));
                    thread_vector.push_back(currentThread);
                    std::vector<thread *> currentThreadVector;
                    currentThreadVector.push_back(currentThread);
                    table_thread_group[tableName].push_back(currentThreadVector);
                    mutex *currentThreadMutex = new mutex;
                    mutex_vector.push_back(currentThreadMutex);
                    thread_join_lock_lock.lock();/////////////
                    thread_join_lock[currentThread] = currentThreadMutex;
                    thread_join_lock_lock.unlock();/////////////
                    table_last_status[tableName] = STATUS_READ;
                } else {
                    (*query).waitThreadsPtr = *(--(--table_thread_group[tableName].end()));
                    (*query).waitThreadsOutputPtr = *(--table_thread_group[tableName].end());
                    counter_lock.lock();//////
                    ++counter;
                    //cerr << counter << endl;
                    counter_lock.unlock();/////
                    thread *currentThread = new thread(&Query::executeAndPrint, move(query));
                    thread_vector.push_back(currentThread);
                    (--table_thread_group[tableName].end())->push_back(currentThread);
                    mutex *currentThreadMutex = new mutex;
                    mutex_vector.push_back(currentThreadMutex);
                    thread_join_lock_lock.lock();/////////////
                    thread_join_lock[currentThread] = currentThreadMutex;
                    thread_join_lock_lock.unlock();/////////////
                    table_last_status[tableName] = STATUS_READ;
                }
            }
            //queryIDCounter++; 
        }
        catch (const std::ios_base::failure &e) {
            // End of input
            break;
        }
        catch (const std::exception &e) {
            cerr << e.what() << endl;
        }
    }
    //while(1); /////////////////
    auto it_out = output_list.begin();
    for (auto it = thread_vector.begin(); it != thread_vector.end(); ++it, ++it_out) {
        thread_join_lock_lock.lock();/////////////
        mutex *lockPtr = thread_join_lock[*it];
        thread_join_lock_lock.unlock();/////////////
        lockPtr->lock();
        if ((*it)->joinable()) {
            (*it)->join();
            //cerr << "join!!!!!" << endl;
        }
        lockPtr->unlock();
        cout << *it_out;
    }
    for (auto it = mutex_vector.begin(); it != mutex_vector.end(); ++it) {
        delete (*it);
    }
    for (auto it = thread_vector.begin(); it != thread_vector.end(); ++it) {
        delete (*it);
    }
    //Database &db = Database::getInstance();
    //db.printAllTable();
    //cerr << "print end" << endl;
    return 0;
}