cmake_minimum_required(VERSION 3.22)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

project(odbc_connector)

find_package(ODBC REQUIRED)

add_executable(odbc_connector main.cpp
              odbc_connector.hpp odbc_connector.cpp
              connection_pool.hpp connection_pool.cpp
              connector_guard.hpp connector_guard.cpp)


target_link_libraries(odbc_connector ODBC::ODBC)