cmake_minimum_required(VERSION 3.10)
project(GraveBot)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenAL REQUIRED)
find_package(CURL REQUIRED)
find_package(JSONCPP REQUIRED)

# Include directories
include_directories(
    ${OPENAL_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/dlls
)

# Source files
set(SOURCES
    dlls/bot_chat.cpp
    dlls/tts_cleanup.cpp
)

# Add the library
add_library(GraveBot SHARED ${SOURCES})

# Link libraries
target_link_libraries(GraveBot
    ${OPENAL_LIBRARIES}
    ${CURL_LIBRARIES}
    ${JSONCPP_LIBRARIES}
)

# Set output directory
set_target_properties(GraveBot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
