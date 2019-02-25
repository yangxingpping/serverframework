cmake_minimum_required(VERSION 3.10)
PROJECT(serverframework)
SET(CMAKE_C_COMPILER "/usr/bin/clang")
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/common/include)

ADD_SUBDIRECTORY(authServer)
ADD_SUBDIRECTORY(connector)
ADD_SUBDIRECTORY(dbManager)
ADD_SUBDIRECTORY(logicServer)
