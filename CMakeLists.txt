cmake_minimum_required(VERSION 3.2)
project(GitNote VERSION 0.1.0)
enable_language(C)

include_directories(src src/hashnote_table lib)

set(SOURCE_FILES 
    src/main.c
    src/git_helpers.c
    src/add.c
    src/complete.c
    src/file_helpers.c
    src/note.c
    src/show.c

    src/hashnote_table/hashnote_table.c
)

add_executable(git-note ${SOURCE_FILES})

# Default build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug) # default to release TODO: Make default debug
endif()

set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -Wall -Wextra -Wpedantic")

add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_MAKE_PROGRAM}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Custom Make Target: `make release`
add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_MAKE_PROGRAM}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)