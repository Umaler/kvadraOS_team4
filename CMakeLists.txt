cmake_minimum_required(VERSION 3.16)
project(MediaSearcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------------------------------------------------------
# 1. nlohmann/json (header-only)
# ---------------------------------------------------------
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

# ---------------------------------------------------------
# 2. cpp-httplib (header-only)
# ---------------------------------------------------------
FetchContent_Declare(
    cpp_httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.43.4
)
FetchContent_MakeAvailable(cpp_httplib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMAGIC REQUIRED IMPORTED_TARGET libmagic)

# ---------------------------------------------------------
# Сборка исполняемого файла
# ---------------------------------------------------------
# В add_executable перечисляем ТОЛЬКО .cpp файлы.
# .hpp файлы подтягиваются автоматически через target_include_directories.
add_executable(mediasearcher
    main.cpp
    system_specific.cpp
    utility.cpp
)

# Указываем директорию с заголовками проекта
target_include_directories(mediasearcher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(mediasearcher PRIVATE
    #nlohmann_json::nlohmann_json
    #httplib
    PkgConfig::LIBMAGIC
)
