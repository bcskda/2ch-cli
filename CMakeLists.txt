cmake_minimum_required(VERSION 3.7)
project(2ch-cli)

if (NOT WIN32)
    string(ASCII 27 Esc)
    set(ColorReset  "${Esc}[m")
    set(ColorBold   "${Esc}[1m")
    set(Red         "${Esc}[31m")
    set(Green       "${Esc}[32m")
    set(Yellow      "${Esc}[33m")
    set(Blue        "${Esc}[34m")
    set(Magenta     "${Esc}[35m")
    set(Cyan        "${Esc}[36m")
    set(White       "${Esc}[37m")
    set(BoldRed     "${Esc}[1;31m")
    set(BoldGreen   "${Esc}[1;32m")
    set(BoldYellow  "${Esc}[1;33m")
    set(BoldBlue    "${Esc}[1;34m")
    set(BoldMagenta "${Esc}[1;35m")
    set(BoldCyan    "${Esc}[1;36m")
    set(BoldWhite   "${Esc}[1;37m")
endif()

# Юзерагент
if ("${CURL_USERAGENT}" STREQUAL "")
    set(CURL_USERAGENT "Linux")
endif()

# Редактор
if ("${DEFAULT_EDITOR}" STREQUAL "")
    set(DEFAULT_EDITOR "nano")
endif()

# Мейл
if ("${DEFAULT_EMAIL}" STREQUAL "")
    set(DEFAULT_EMAIL "")
endif()

message("${Cyan}Default useragent is \"${Magenta}${CURL_USERAGENT}${Cyan}\"${ColorReset}")
message("${Cyan}Default editor is \"${Magenta}${DEFAULT_EDITOR}${Cyan}\"${ColorReset}")
message("${Cyan}Default email is \"${Magenta}${DEFAULT_EMAIL}${Cyan}\"${ColorReset}")


find_package(PkgConfig)

pkg_check_modules(ALL_LIBS REQUIRED
        libcurl
        jsoncpp
        ncursesw
        caca)

link_directories(${ALL_LIBS_LIBRARY_DIRS})
include_directories(${ALL_LIBS_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2 -fPIC -pipe -Wall -Wno-pointer-arith")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -DMAKABA_DEBUG")

set(SOURCE_FILES
        src/error.cpp
        src/error.h
        src/external.cpp
        src/external.h
        src/API.cpp
        src/API.h
        src/makaba.cpp
        src/makaba.h
        src/cache.cpp
        src/cache.h
        src/app.cpp
        src/app.h
        src/main.cpp)

add_executable(2ch-cli ${SOURCE_FILES})
target_link_libraries(2ch-cli ${ALL_LIBS_LIBRARIES})

# Костыльно, но иначе CMake ругается.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCURL_UA=\"\\\"${CURL_USERAGENT}\\\"\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEFAULT_EDITOR=\"\\\"${DEFAULT_EDITOR}\\\"\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEFAULT_EMAIL=\"\\\"${DEFAULT_EMAIL}\\\"\"")

install(TARGETS 2ch-cli DESTINATION bin)
