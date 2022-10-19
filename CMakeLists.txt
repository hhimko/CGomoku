cmake_minimum_required(VERSION 3.7.0)
project(CGomoku VERSION 0.1.0)

# Set executable output path to inside the build tree
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set executable name
set(BIN_NAME Gomoku)

file(GLOB SOURCES "src/*.c")
add_executable(${BIN_NAME} ${SOURCES})

# Set compiler warning flags
target_compile_options(${BIN_NAME} PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /WX>)
target_compile_options(${BIN_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)

# Link CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Add SDL2 libs
find_package(SDL2 REQUIRED)
target_link_libraries(${BIN_NAME} SDL2::Main)

target_include_directories(${BIN_NAME} PUBLIC src)
target_include_directories(${BIN_NAME} PUBLIC ${SDL2_INCLUDE_DIR})

# Copy asset directory to executable path
file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
