cmake_minimum_required(VERSION 3.10)
PROJECT(serverframework)
SET(CMAKE_C_COMPILER "/usr/bin/clang")
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")

add_definitions(-DOTL_ODBC_UNIX)
add_definitions(-DNEWUWEBSCKETS)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/common/include)

IF(APPLE)
    add_definitions(-DSPDLOG_FMT_EXTERNAL)
    include_directories(/Users/alqazy/source/vcpkg/installed/x64-osx/include)
    include_directories(/usr/local/include)
    link_directories(/Users/alqazy/source/vcpkg/installed/x64-osx/lib)
    link_directories(/usr/local/lib)
ELSEIF(UNIX)
    include_directories(/root/source/vcpkg/installed/x64-linux/include)
    include_directories(/root/source/vcpkg/installed/x64-linux/include/uSockets)
    include_directories(/root/source/vcpkg/installed/x64-linux/include/newuWS)
    link_directories(/root/source/vcpkg/installed/x64-linux/debug/lib)
ENDIF()

link_libraries(uSockets)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/common/src DIR_SRCS)

ADD_SUBDIRECTORY(authServer)
ADD_SUBDIRECTORY(connector)
ADD_SUBDIRECTORY(dbManager)
ADD_SUBDIRECTORY(logicServer)
