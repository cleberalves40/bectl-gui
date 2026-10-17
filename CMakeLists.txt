cmake_minimum_required(VERSION 3.16)
project(bectl_gui VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 (FreeBSD: pkg install qt6-base qt6-tools)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Lógica de integração com o bectl(8), compartilhada pela GUI e pelos testes.
add_library(bectl_logic STATIC
    src/logic/bectlmanager.cpp
    src/logic/bectlmanager.h
)
target_include_directories(bectl_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/logic)
target_link_libraries(bectl_logic PUBLIC Qt6::Core)

set(SOURCES
    src/main.cpp
    src/gui/mainwindow.cpp
    src/gui/themes.cpp
    src/i18n/i18n.cpp
)

set(HEADERS
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
    bectl_logic
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_options(bectl_gui PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
)

# Fonte única da versão: project(... VERSION ...)
target_compile_definitions(bectl_gui PRIVATE BECTL_GUI_VERSION="${PROJECT_VERSION}")

# Teste de integração contra o bectl(8) real (veja tests/freebsd-e2e.sh).
option(BECTL_GUI_BUILD_TESTS "Compila tst_bectlmanager" OFF)
if(BECTL_GUI_BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    enable_testing()
    add_executable(tst_bectlmanager tests/tst_bectlmanager.cpp)
    target_link_libraries(tst_bectlmanager PRIVATE bectl_logic Qt6::Test)
    target_compile_options(tst_bectlmanager PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
    )
    add_test(NAME tst_bectlmanager COMMAND tst_bectlmanager)
endif()

# Instalação
install(TARGETS bectl_gui RUNTIME DESTINATION bin)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bectl_gui.desktop")
    install(FILES bectl_gui.desktop DESTINATION share/applications)
endif()

# Mensagem útil
message(STATUS "bectl_gui (C++/Qt6) – configure com: cmake -B build -S . && cmake --build build")
