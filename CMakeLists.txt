# ----------------------------------------------------------------------------
# минимальная версия CMake, способная переварить этот файл
# ----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}) 

if (MSVC)
    add_subdirectory(glfw)
endif()
add_subdirectory(zlib)

set(project_name TrackerGenerator)

project(${project_name})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -std=c++11")
#----------------------------------------------------------------------------  
# ищем все .cpp и .h файлы и добавляем их в наш проект 
# ----------------------------------------------------------------------------
SET(SOURCE
main/main.cpp
main/nativefiledialog/src/nfd_common.c
ImGuiColorTextEdit/TextEditor.cpp
imgui/examples/libs/gl3w/GL/gl3w.c
imgui/examples/imgui_impl_glfw.cpp
imgui/examples/imgui_impl_opengl3.cpp
imgui/imgui.cpp
imgui/imgui_draw.cpp
imgui/imgui_widgets.cpp

)

SET(HEADERS
main/nativefiledialog/src/include/nfd.h
ImGuiColorTextEdit/TextEditor.h
)

if (CMAKE_COMPILER_IS_GNUCXX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    #find_package(zlib REQUIRED)

    if (GTK3_FOUND)
        include_directories(${GTK3_INCLUDE_DIRS})
        LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})
        add_definitions(${GTK3_CFLAGS_OTHER})
    else()
        message("GTK_FOUND")
    endif()

    SET(SOURCE ${SOURCE} main/nativefiledialog/src/nfd_gtk.c)
elseif (MSVC)
    SET(SOURCE ${SOURCE} main/nativefiledialog/src/nfd_win.cpp)
endif()

file(GLOB ADD_HEADERS "./imgui/*.h*")
SET(HEADERS ${HEADERS} ${ADD_HEADERS})

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
INCLUDE_DIRECTORIES("./")
INCLUDE_DIRECTORIES("imgui")
INCLUDE_DIRECTORIES("imgui/examples")
INCLUDE_DIRECTORIES("imgui/examples/libs/gl3w")
INCLUDE_DIRECTORIES("imgui/examples/libs/glfw/include")
INCLUDE_DIRECTORIES("main")
INCLUDE_DIRECTORIES("main/nativefiledialog/src/include")
INCLUDE_DIRECTORIES("main/nativefiledialog/src")
INCLUDE_DIRECTORIES("ImGuiColorTextEdit")
include_directories(${CMAKE_SOURCE_DIR}/zlib)
include_directories(
  file_template_parser/include
  TinyXML
)

# Create file template parser library
add_library(ft_parser STATIC
   
  TinyXML/tinystr.cpp
  TinyXML/tinyxml.cpp
  TinyXML/tinyxmlerror.cpp
  TinyXML/tinyxmlparser.cpp 
  file_template_parser/src/template_container.cpp
  file_template_parser/src/template_management_tools.cpp
)
# ----------------------------------------------------------------------------  
# создаем проект
# ----------------------------------------------------------------------------
ADD_EXECUTABLE(${project_name} ${SOURCE} ${HEADERS})

# ----------------------------------------------------------------------------
# и Lib-ы opencv 
# ----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCXX)
    TARGET_LINK_LIBRARIES(${project_name} ${GTK3_LIBRARIES} glfw zlib ft_parser ${Boost_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} dl)

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/GUIConfig.xml DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/misc/fonts/a_FuturaOrto.TTF DESTINATION ${CMAKE_BINARY_DIR})

elseif (MSVC)
TARGET_LINK_LIBRARIES(${project_name}
                        optimized ${CMAKE_BINARY_DIR}/Release/glfw3.lib
                        debug 	${CMAKE_BINARY_DIR}/Debug/glfw3.lib
                        optimized ${CMAKE_BINARY_DIR}/Release/zlib.lib
                        debug     ${CMAKE_BINARY_DIR}/Debug/zlib.lib
                        ft_parser
                        opengl32.lib
                        )

                    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/GUIConfig.xml DESTINATION ${CMAKE_BINARY_DIR}/Debug)
                    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/misc/fonts/a_FuturaOrto.TTF DESTINATION ${CMAKE_BINARY_DIR}/Release)
endif()
