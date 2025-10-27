cmake_minimum_required(VERSION 3.16)
project(PhotoEditor C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(photo_editor
    src/main.c
    src/library.c
    src/pgm.c
    src/image_processing.c)

target_include_directories(photo_editor PRIVATE include)

if (MSVC)
    target_compile_options(photo_editor PRIVATE /W4 /WX)
else ()
    target_compile_options(photo_editor PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()
