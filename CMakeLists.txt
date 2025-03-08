cmake_minimum_required(VERSION 3.2)
project(GitNote VERSION 0.1.0)
enable_language(C)

set(SOURCE_FILES 
    src/main.c
    src/git_helpers.c
    src/add.c
    src/complete.c
    src/file_helpers.c
    src/note.c
    src/show.c
)

add_executable(git-note ${SOURCE_FILES})