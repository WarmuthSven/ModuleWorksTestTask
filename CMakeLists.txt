cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(cutSphereMove C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB cutSphereMove_SRC
  "library/include/**/*.hpp"
  "library/src/**/*.cpp"
  "*.cpp"
)
add_executable(cutSphereMove ${cutSphereMove_SRC})

target_include_directories(cutSphereMove
        PRIVATE 
            .
            library/include/)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${cutSphereMove_SRC})