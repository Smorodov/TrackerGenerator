# ----------------------------------------------------------------------------
# минимальная версия CMake, способная переварить этот файл
# ----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}) 

set(project_name SignalsSlotsBoostThreadsTemplate)

project(${project_name})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -std=c++11")
#----------------------------------------------------------------------------  
# ищем все .cpp и .h файлы и добавляем их в наш проект 
# ----------------------------------------------------------------------------
SET(SOURCE
src/main.cpp
src/BaseWorkerClass.cpp
src/WorkerClass.cpp
)

SET(HEADERS
src/BaseWorkerClass.h
src/WorkerClass.h
src/Observer.h
src/Observable.h
)

SOURCE_GROUP("Source Files" FILES ${SOURCE})
SOURCE_GROUP("Header Files" FILES ${HEADERS})

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_STATIC_RUNTIME   ON)
find_package(Boost 1.54 REQUIRED COMPONENTS system thread filesystem regex)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
get_directory_property(_my_link_dirs LINK_DIRECTORIES)
message(STATUS "_my_link_dirs = ${_my_link_dirs}") 
# ----------------------------------------------------------------------------   
# добавляем include директории
# ----------------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/src)


# ----------------------------------------------------------------------------  
# создаем проект
# ----------------------------------------------------------------------------
  ADD_EXECUTABLE( ${project_name}
  				  ${SOURCE}
  				  ${HEADERS}
  				  )

# ----------------------------------------------------------------------------
# Lib-ы
# ----------------------------------------------------------------------------
# TARGET_LINK_LIBRARIES(${project_name}  )
