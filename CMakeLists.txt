cmake_minimum_required(VERSION 3.4)
project(p2)

set(CMAKE_CXX_STANDARD 14)

find_package( Threads )

include_directories(includes lib src lib/includes)

set(SOURCE_FILES
        includes/data_query.h
        includes/formatter.h
        includes/management_query.h
        includes/query.h
        includes/query_builders.h
        includes/query_parser.h
        lib/includes/db.h
        lib/includes/db_table.h
        lib/includes/db_table_impl.hpp
        lib/includes/formatter.h
        lib/includes/query_results.h
        lib/includes/uexception.h
        lib/db.cpp
        lib/db_table.cpp
        lib/query_results.cpp
        src/data_query.cpp
        src/main.cpp
        src/management_query.cpp
        src/query.cpp
        src/query_builders.cpp
        src/query_parser.cpp
        # main_test_table.cpp
        )

find_package( Threads )

add_executable(p2 ${SOURCE_FILES})
target_link_libraries(p2 ${CMAKE_THREAD_LIBS_INIT})