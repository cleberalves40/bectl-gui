cmake_minimum_required(VERSION 3.16)
project(bectl_gui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 (FreeBSD: pkg install qt6-base)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Lógica de integração com o bectl(8) e tradução, compartilhadas pela GUI e
# pelos testes.
add_library(bectl_logic STATIC
    src/logic/bectlmanager.cpp
    src/logic/bectlmanager.h
    src/i18n/i18n.cpp
    src/i18n/i18n.h
)
target_include_directories(bectl_logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logic
    ${CMAKE_CURRENT_SOURCE_DIR}/src/i18n
)
target_link_libraries(bectl_logic PUBLIC Qt6::Core)

# Interface, compartilhada pelo executável e pelo teste da interface.
add_library(bectl_ui STATIC
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    src/gui/themes.cpp
    src/gui/themes.h
)
target_include_directories(bectl_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gui)
target_link_libraries(bectl_ui PUBLIC bectl_logic Qt6::Gui Qt6::Widgets)

add_executable(bectl_gui src/main.cpp)
target_link_libraries(bectl_gui PRIVATE bectl_ui)

foreach(target bectl_logic bectl_ui bectl_gui)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
    )
endforeach()

# Fonte única da versão: project(... VERSION ...)
target_compile_definitions(bectl_gui PRIVATE BECTL_GUI_VERSION="${PROJECT_VERSION}")

# tst_validation e tst_mainwindow rodam em qualquer sistema; tst_bectlmanager
# é o teste de integração contra o bectl(8) real (veja tests/freebsd-e2e.sh).
option(BECTL_GUI_BUILD_TESTS "Compila os testes" OFF)
if(BECTL_GUI_BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    enable_testing()
    foreach(test tst_validation tst_mainwindow tst_bectlmanager)
        add_executable(${test} tests/${test}.cpp)
        target_link_libraries(${test} PRIVATE bectl_ui Qt6::Test)
        target_compile_options(${test} PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
        )
        add_test(NAME ${test} COMMAND ${test})
    endforeach()
endif()

# Instalação
install(TARGETS bectl_gui RUNTIME DESTINATION bin)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bectl_gui.desktop")
    install(FILES bectl_gui.desktop DESTINATION share/applications)
endif()

# Mensagem útil
message(STATUS "bectl_gui (C++/Qt6) – configure com: cmake -B build -S . && cmake --build build")
