# Specify minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Set project name and version
project(Clerk VERSION 1.0)

# Set C++17 as the standard
set(CMAKE_CXX_STANDARD 17)
# Make C++17 standard required
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Disable compiler extensions
set(CMAKE_CXX_EXTENSIONS OFF)

# Set Debug mode as default if build type is not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Debug mode: Show all warnings (-Wall), extra warnings (-Wextra),
    # include debugging information (-g)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
else()
    # Release mode: Optimization level 3 (-O3)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Find all .cpp files in src/ directory
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/db/*.cpp"
    "src/mqtt/*.cpp"
)

# Create executable named clerk
add_executable(${PROJECT_NAME} ${SOURCES})

# Add global include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add include directories for header files
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/db
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mqtt
)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

# Enable export of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Boost library for uuid component
find_package(Boost REQUIRED COMPONENTS system)

# Find Paho MQTT C library
find_library(PAHO_MQTT_C NAMES libpaho-mqtt3as.so paho-mqtt3as
    PATHS /usr/lib /usr/lib/x86_64-linux-gnu
    REQUIRED
)

find_library(PAHO_MQTT_CPP NAMES libpaho-mqttpp3.so paho-mqttpp3
    PATHS /usr/lib /usr/lib/x86_64-linux-gnu
    REQUIRED
)

# Find PostgreSQL C++ package
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Link packages and libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::boost
    ${PAHO_MQTT_CPP}
    ${PAHO_MQTT_C}
    ${PQXX_LIBRARIES}
)
