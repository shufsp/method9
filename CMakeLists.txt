cmake_minimum_required(VERSION 3.20)
project(method9)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server-crawler
        searcher/crawler.cpp
        searcher/crawler_network.cpp
)

target_link_libraries(server-crawler
        fmt
        boost_system
        boost_regex
)

add_executable(server-dat-creator
        nbt/nbt_create.cpp
        nbt/nbt_adapt.cpp
        nbt/NBTWriter.cpp
)

target_link_libraries(server-dat-creator
        fmt
)