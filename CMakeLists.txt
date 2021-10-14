#sudo apt install libasio-dev
#sudo apt install libblkid-dev e2fslibs-dev libboost-all-dev libaudit-dev

cmake_minimum_required(VERSION 3.16)
include(CMakeDependentOption)

set(LIB_NAME "tsai-common-open")

message("----------------------------")
message("        ${LIB_NAME}")
message("----------------------------")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "Set CXX standard.")

set(PROJ_NAME "console")
project(${PROJ_NAME})

add_compile_options(-Wall)
add_compile_options(-g)
message(STATUS "Set compile options.")

SET(BUILD_STATIC_LIBS ON) 

# Threading 
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
message(STATUS "Initalised lpthread.")

find_package( PkgConfig REQUIRED)
message(STATUS "PkgConfig found and added.")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.71.0 REQUIRED COMPONENTS system thread regex)
if(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

set(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

message(STATUS "Boost added.")
message(STATUS "  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "  Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "  Boost_VERSION: ${Boost_VERSION}")

add_compile_options(-Wall)
message("-- Set compile options to report for all warnings.")

add_library(
    ${LIB_NAME} STATIC
    include/BOOST/ConnectionTCP.cpp
    include/rapidxml-1.13/rapidxml_iterators.hpp
    include/rapidxml-1.13/rapidxml_print.hpp
    include/rapidxml-1.13/rapidxml_utils.hpp
    include/rapidxml-1.13/rapidxml.hpp
    include/JetsonGPIO/include/gpio_event.h
    include/JetsonGPIO/include/gpio_pin_data.h
    include/JetsonGPIO/include/JetsonGPIO.h
    include/JetsonGPIO/include/Model.h
    include/JetsonGPIO/include/PythonFunctions.h
    include/JetsonGPIO/src/gpio_event.cpp
    include/JetsonGPIO/src/gpio_pin_data.cpp
    include/JetsonGPIO/src/JetsonGPIO.cpp
    include/JetsonGPIO/src/PythonFunctions.cpp
    include/JetsonGPIO/src/test.cpp
)
message("-- Created library.")


# add the executable
add_executable(${PROJ_NAME} main.cpp)
message(STATUS "Added executable.")

target_link_libraries(${PROJECT_NAME} -Boost)
target_link_libraries(${PROJECT_NAME} ${LIB_NAME} )
message(STATUS "Linked other libraries.")
