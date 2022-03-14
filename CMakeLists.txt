cmake_minimum_required(VERSION 3.21)
project(Lab_compiler C)

set(CMAKE_C_STANDARD 99)


include_directories(${PROJECT_SOURCE_DIR}/Code)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_FLAGS "-lfl -ly")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_C_FLAGS "-LC:/GnuWin32/lib/")
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

add_custom_target(TEMP)

add_custom_command(TARGET TEMP
PRE_BUILD
COMMAND flex -o ${PROJECT_SOURCE_DIR}/Code/lex.yy.c ${PROJECT_SOURCE_DIR}/Code/lexical.l
COMMAND bison -o ${PROJECT_SOURCE_DIR}/Code/syntax.tab.c -d -v ${PROJECT_SOURCE_DIR}/Code/syntax.y
)


file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/Code/*.c)
file(GLOB UNUSED ${PROJECT_SOURCE_DIR}/Code/lex.yy.c)

list(REMOVE_ITEM SRC_LIST ${UNUSED})

add_executable(parse ${SRC_LIST})
message("files:${SRC_LIST}")
message("Exclude:${UNUSED}")

