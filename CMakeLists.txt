cmake_minimum_required(VERSION 3.14)

project(Event_driven_framework VERSION 1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(Event_driven_framework 
  main.cpp
  src/ao_service.cpp
)

set_target_properties(Event_driven_framework PROPERTIES LINK_LIBRARIES -pthread)
#target_include_directories(Event_driven_framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
