cmake_minimum_required(VERSION 3.14)

project(Server LANGUAGES CXX)

add_executable(Server main.cpp
    server.h server.cpp)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(BOOST_ROOT "C:/libraries/boost_1_85_0")
set(BOOST_INCLUDEDIR "C:/libraries/boost_1_85_0")
set(BOOST_LIBRARYDIR "C:/libraries/boost_1_85_0/lib64-msvc-14.3")


#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
#target_link_libraries(Server Qt${QT_VERSION_MAJOR}::Core)

find_package(Boost 1.85.0 COMPONENTS thread chrono REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(Server ${Boost_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS Server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

