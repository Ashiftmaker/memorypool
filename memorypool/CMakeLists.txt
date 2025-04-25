cmake_minimum_required(VERSION 3.10)

project(memorypoll)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} 
    memorypool.cpp 
    memoryblock.cpp
    blocknodepool.cpp 
    main.cpp
    memoryblock.h
    blocknode.h
    memorypool.h
    blocknodepool.h
)