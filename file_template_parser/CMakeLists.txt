cmake_minimum_required(VERSION 2.8.3)
project(file_template_parser)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})


set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_STATIC_RUNTIME   ON)
find_package(Boost 1.54 REQUIRED COMPONENTS system thread filesystem regex)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
get_directory_property(_my_link_dirs LINK_DIRECTORIES)
message(STATUS "_my_link_dirs = ${_my_link_dirs}") 

add_compile_options(-std=c++11)

include_directories(
  include
  ./../TinyXML
)

# Create file template parser library
add_library(ft_parser STATIC
   
  ./../TinyXML/tinystr.cpp
  ./../TinyXML/tinyxml.cpp
  ./../TinyXML/tinyxmlerror.cpp
  ./../TinyXML/tinyxmlparser.cpp 
  src/template_container.cpp
  src/template_management_tools.cpp
)

target_link_libraries(ft_parser ${TinyXML_LIBRARIES})

# Create a testing code
# add_executable(template_import_test src/template_import_test.cpp)
# target_link_libraries(template_import_test ft_parser )
