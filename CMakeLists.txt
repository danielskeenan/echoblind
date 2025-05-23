# Pin minimum CMake version to latest Ubuntu LTS release.
# https://packages.ubuntu.com/search?searchon=names&keywords=cmake
cmake_minimum_required(VERSION 3.28)
project(echoblind
        DESCRIPTION "Edit Echo panel information offline."
        VERSION 0.1.1
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/danielskeenan/echoblind"
)
set(PROJECT_DISPLAY_NAME "Echo Blind")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_ORGANIZATION_NAME "dankeenan")
set(PROJECT_ORGANIZATION_DOMAIN "dankeenan.org")
string(TIMESTAMP PROJECT_BUILD_TIMESTAMP "%s" UTC)
find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" log -n1 --format=%H
        OUTPUT_VARIABLE PROJECT_COMMIT_SHA
        COMMAND_ERROR_IS_FATAL ANY
)
string(STRIP "${PROJECT_COMMIT_SHA}" PROJECT_COMMIT_SHA)
execute_process(COMMAND "${GIT_EXECUTABLE}" tag --points-at=HEAD
        OUTPUT_VARIABLE PROJECT_COMMIT_TAG
        COMMAND_ERROR_IS_FATAL ANY
)
string(STRIP "${PROJECT_COMMIT_TAG}" PROJECT_COMMIT_TAG)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)

option(BUILD_EXEC "Build the executable program.  You probably want to do this." ON)
if (BUILD_EXEC)
    include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")

    set(QT_VERSION_MAJOR 6)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    # This is more portable across compilers compared to other methods
    string(TOUPPER "${CMAKE_SYSTEM_NAME}" PLATFORM_NAME)
    set(PLATFORM_NAME "PLATFORM_${PLATFORM_NAME}")
    add_compile_definitions("${PLATFORM_NAME}")

    find_package(Qt6 COMPONENTS Core Svg Widgets Xml REQUIRED)
    add_compile_definitions(QT_NO_KEYWORDS)
    qt_standard_project_setup()
    add_subdirectory(src)

    include(CTest)
    if (BUILD_TESTING)
        add_subdirectory(test)
    endif ()

    option(BUILD_PACKAGE "Build packages/installers usable on this platform." OFF)
    if (BUILD_PACKAGE)
        add_subdirectory(install)
    endif ()
endif ()

option(BUILD_DOC "Build documentation" ON)
if (BUILD_DOC)
    #    add_subdirectory(doc)
endif ()
