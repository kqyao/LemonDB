# README

> This is the new version of `lemondb`.

## Multi-threading Strategy

The multi-threading is mostly done in `main.cpp` and `int main()` function. The general idea is one thread for one query (except `QUIT`), and trying to let the threads run concurrently on the premise that the result is correct.  

### Separating different queries

We categorize the queries into 4 types: 

- **Read-only**, which contains `DUMP`, `SELECT`, `SUM`, `MIN`, and `MAX`;
- **Write within one table (may also read)**, which contains `LOAD`, `DROP`, `TRUNCATE`, `DELETE`, `INSERT`, `UPDATE`, `DUPLICATE`, `SWAP`, `ADD`, and `SUB`; 
- **Read one table and write another table**, which only contains `COPYTABLE`;
- **Quit**, which only contains `QUIT`.

In order to determine the query type and the name(s) of involved table(s) of a query in `main` function, we add several public methods to the `Query` class: 

```c++
virtual string commandName() = 0; // return the command name of a query, like "LOAD" or "SELECT", etc. 
virtual string getTableName() = 0;  // return the name of the table needed or affected
virtual string getTableNameSecond() = 0; // only for COPYTABLE, because COPYTABLE involves 2 tables
```

We implement the methods in the derived classes, like `CopyTableQuery`, `SumQuery`, etc. Therefore, we become able to get the query type and involved tables in `main` function. By doing that, we can treat different kinds of queries as following: 

- If the query is `QUIT`, then jump out of the loop;
- Else, get the name of the table that the query involves, and the permission (read-only, or write); (If the query is `COPYTABLE`, it will read one table and write another table);
- For each table, the adjacent read-only tasks can be executed concurrently, and the write task must be executed alone;
- For different tables, the tasks can be executed concurrently. 

The threads are designed in the structure of

```
+-----------------------------------+    ...    +-----------------------------------+       |
|              table 1              |           |              table n              |       |
+-----------------------------------+           +-----------------------------------+       | t
|            write task             |           |            write task             |       | i
+-------------+-------------+-------+           +-------------+-------------+-------+       | m
| read task 1 | read task 2 |  ...  |           | read task 1 | read task 2 |  ...  |       | e
+-------------+-------------+-------+           +-------------+-------------+-------+       | 
|            write task             |           |            write task             |       |
			    ...                   			               ...                        ↓
```

We implement the structure using a global variable

```c++
unordered_map<string, vector<vector<thread *>>> table_thread_group;
```

For better explanation, we define a `concurrent group` as a group of threads in a table that can work concurrently. For example, a write task in table 1 forms a concurrent group; read task 1, 2, ... in table 1 forms another concurrent group. 

The unordered_map maps table name to `vector<vector<thread *>>`, where `vector<thread *>` is a concurrent group of a table, and `vector<vector<thread *>>` is the vector of concurrent groups of a table. 

### Keeping the order of execution

In order to keep the order of execution, every concurrent group must wait for the completion of previous concurrent group. Therefore, a thread must know which threads to wait for. We add two attributes

```c++
vector<thread*> waitThreadsPtr = vector<thread*>(); // wait the threads in the vector before execute
vector<thread*> waitThreadsSecondPtr = vector<thread*>(); // only for COPYTABLE
```

 to class `Query`. We assign the value to the attribute(s) in `main` function before every thread is `new`ed. A new thread will join all the threads in the two vectors before executing (see function `executeAndPrint` in `query.h`). 

###Output strategy & implementation

We choose the strategy to read in all the queries, then output all the results, so it is not same as previous `lemondb`'s immediate response. 

We insert the output string of every query to the output list `list<string> output_list` (global variable) to keep the order of output. 

### Encountering cost of too much threads

In the real testing environment, the machine may complain `Temporary resource unavailable` if the thread number is not controlled. Our resolution is to add a global `counter` for the running threads. If the thread number is greater than specified, we stop executing new queries until `counter` get smaller. 

### Cleanup after all inputs

Go through `thread_vector` to join all the threads, go through `output_list` to finish output. By checking the `thread_vector`'s threads' job, we go pass or join them in case the output is still unavailable.

##Make Method

Please compile the whole program with `make` or `make re`. 

## Some Naïve Performance Test

We test our multi-thread program on a 2 core 4 thread CPU computer, platform Arch Linux.

In multiple table test cases, the time elapsed is less than $50\%$ of the original `lemondb`;

In single table read-only test cases, the time elapsed is less than $50\%$ of the original `lemondb`;

In single table read-and-write-mixed or write-only test cases, the time elapsed is about $110\%$ of the original `lemondb`. 

## Overcoming Some Common Problems in Multi-threading

- Race condition:

  If a thread accesses or writes one shared variable when another thread is writing it, race condition may occur. We simply use mutexes (the global variables with suffix `_lock`) to prevent race conditions. 

  If a thread accesses or writes one table when another thread is writing it, race condition may occur. Our  multi-threading strategy guarantees that this kind of race condition will not occur. 

- Deadlocks:

  In our code, there is no loop in thread resources, because a thread will only join the threads that are ahead of it. Also, in our code there is no case where two mutexes are acquired at the same time. 

- The multi threading output ordering problem:

  If the problem is implemented with multi thread method, then we have to face the fact that different job will be finished in different time length. Thus the order of output should be preserved.

  We derive 2 solutions:

  - The output is temporarily stored in a list or a vector, if it is filled with no "hole" inside, then it is cleared. The output list is then updated.
  - The output in a vector is fixed, then we just wait until all the threads finish its job, output the vector content.

  We choose the later one since it may cost less in time and compute resources.

- If too much thread is called, the problem may not happen on windows, but `linux` may occur annoying `Temporary resource unavailable` complain, thus the program will not perform in the way we want.

  We derive some kind of solution by fitting with hardware. We control the running thread number by adding a global counter. If the running counter is larger then some hardware specified value, new queries will wait until the counter gets smaller.

## Known Bug

- Sometimes the program stops working (CPU utilization becomes 0), but the program does not crash. It is just running and seems doing nothing. This situation happens about $5\%$ on two team members' `Bash On Win` when running large test cases (like `many_insert_delete.query`). But it never happened on another team member's Arch Linux OS. The source of the bug has not been found. 

## Things that need improving

- In single table read-and-write-mixed or write-only test cases, the time elapsed is even more than the original `lemondb`. The performance can be improved by improving the multi-threading strategy: use *fields* as the operating unit instead of *tables*. That is, a thread is put into a new concurrent group if and only if 1. the fields that the thread will read are modified by the current concurrent group, or 2. the fields that the thread will modify are read or modified by the current concurrent group. Due to time limitation, we haven't implemented the new strategy correctly. 
- We output all the information at the same time after all the queries are read. This is not feasible in reality. We can try a different output strategy: create a thread for outputting. The thread is responsible for checking whether a query is completed. If completed, then print the information, and check whether the next query is completed. 

