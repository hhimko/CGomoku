cmake_minimum_required(VERSION 3.12.0)
project(CGomoku VERSION 0.1.0)

# Set executable output path to inside the build tree
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set executable name
set(BIN_NAME Gomoku)

# Add project sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c") # https://stackoverflow.com/questions/1027247/is-it-better-to-specify-source-files-with-glob-or-each-file-individually-in-cmak
add_executable(${BIN_NAME} ${SOURCES})
target_include_directories(${BIN_NAME} PUBLIC src/)

# Set compiler flags
target_compile_options(${BIN_NAME} PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /WX>)
target_compile_options(${BIN_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)

# Link CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Add SDL2 libs
if(WIN32)
    set(SDL2_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL2)
    find_package(SDL2 QUIET)

    # Fall back to looking for SDL2 locally
    if(NOT SDL2_FOUND)
        set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL2)

        if(MINGW)
            # Support both 32 and 64 bit builds
            if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
                set(SDL2_INCLUDE_DIRS ${INCLUDE_DIR}/x86_64-w64-mingw32/include/SDL2)
                set(SDL2_LIBRARIES ${INCLUDE_DIR}/x86_64-w64-mingw32/lib/libSDL2.a;${INCLUDE_DIR}/x86_64-w64-mingw32/lib/libSDL2main.a)
            else()
                set(SDL2_INCLUDE_DIRS ${INCLUDE_DIR}/i686-w64-mingw32/include/SDL2)
                set(SDL2_LIBRARIES ${INCLUDE_DIR}/i686-w64-mingw32/lib/libSDL2.a;${INCLUDE_DIR}/i686-w64-mingw32/lib/libSDL2main.a)
            endif()
        else() # Not using MinGW
            set(SDL2_INCLUDE_DIRS ${INCLUDE_DIR}/include)
            # Support both 32 and 64 bit builds
            if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
                set(SDL2_LIBRARIES ${INCLUDE_DIR}/lib/x64/SDL2.lib;${INCLUDE_DIR}/lib/x64/SDL2main.lib)
                file(COPY ${INCLUDE_DIR}/lib/x64/SDL2.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
                file(COPY ${INCLUDE_DIR}/lib/x64/SDL2.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
            else()
                set(SDL2_LIBRARIES ${INCLUDE_DIR}/lib/x86/SDL2.lib;${INCLUDE_DIR}/lib/x86/SDL2main.lib)
                file(COPY ${INCLUDE_DIR}/lib/x86/SDL2.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
                file(COPY ${INCLUDE_DIR}/lib/x86/SDL2.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
            endif()
        endif() # if(MINGW)
        string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
    endif() # if(NOT SDL2_FOUND)

    # Find SDL2_mixer
    set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL2_mixer)
    set(SDL2_MIXER_PATH ${INCLUDE_DIR})

    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        set(SDL2_MIXER_LIBRARY ${INCLUDE_DIR}/x86_64-w64-mingw32/lib/libSDL2_mixer.dll.a)
        set(SDL2_MIXER_INCLUDE_DIR ${INCLUDE_DIR}/x86_64-w64-mingw32/include/SDL2)
        file(COPY ${INCLUDE_DIR}/x86_64-w64-mingw32/bin/SDL2_mixer.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
        file(COPY ${INCLUDE_DIR}/x86_64-w64-mingw32/bin/SDL2_mixer.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    else()
        set(SDL2_MIXER_LIBRARY ${INCLUDE_DIR}/i686-w64-mingw32/lib/libSDL2_mixer.dll.a)
        set(SDL2_MIXER_INCLUDE_DIR ${INCLUDE_DIR}/i686-w64-mingw32/include/SDL2)
        file(COPY ${INCLUDE_DIR}/i686-w64-mingw32/bin/SDL2_mixer.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
        file(COPY ${INCLUDE_DIR}/i686-w64-mingw32/bin/SDL2_mixer.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    endif()

    find_package(SDL2_mixer REQUIRED)

else() # Target system is not Windows
    find_package(SDL2 REQUIRED)
    find_package(SDL2_mixer REQUIRED)
endif()

# Link libm if found
set(LIBM_LIBRARIES "")
include(CheckLibraryExists)
check_library_exists(m pow "" LIBM)
if(LIBM)
    list(APPEND LIBM_LIBRARIES "m")
endif()

target_link_libraries(${BIN_NAME} ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${LIBM_LIBRARIES})
target_include_directories(${BIN_NAME} PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})


# Copy asset directory to executable path
file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
