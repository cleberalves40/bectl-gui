cmake_minimum_required(VERSION 3.16)
project(bectl_gui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 (FreeBSD: pkg install qt6-base qt6-tools)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    src/main.cpp
    src/logic/bectlmanager.cpp
    src/gui/mainwindow.cpp
    src/gui/themes.cpp
    src/i18n/i18n.cpp
)

set(HEADERS
    src/logic/bectlmanager.h
    src/gui/mainwindow.h
    src/gui/themes.h
    src/i18n/i18n.h
)

add_executable(bectl_gui ${SOURCES} ${HEADERS})

target_include_directories(bectl_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logic
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/i18n
)

target_link_libraries(bectl_gui PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_options(bectl_gui PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
)

# Instalação
install(TARGETS bectl_gui RUNTIME DESTINATION bin)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bectl_gui.desktop")
    install(FILES bectl_gui.desktop DESTINATION share/applications)
endif()

# Mensagem útil
message(STATUS "bectl_gui (C++/Qt6) – configure com: cmake -B build -S . && cmake --build build")
