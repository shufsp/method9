cmake_minimum_required(VERSION 3.20)
project(method9)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system regex program_options)

add_executable(server-crawler
        ${PROJECT_SOURCE_DIR}/searcher/search.cpp
        ${PROJECT_SOURCE_DIR}/searcher/crawler_network.cpp
        ${PROJECT_SOURCE_DIR}/searcher/crawler_options.cpp
)
target_link_libraries(server-crawler Boost::system Boost::regex Boost::program_options)
target_link_libraries(server-crawler fmt)

add_executable(server-dat-creator
        ${PROJECT_SOURCE_DIR}/nbt/nbt_create.cpp
        ${PROJECT_SOURCE_DIR}/nbt/nbt_adapt.cpp
        ${PROJECT_SOURCE_DIR}/nbt/NBTWriter.cpp
)
target_link_libraries(server-dat-creator fmt)