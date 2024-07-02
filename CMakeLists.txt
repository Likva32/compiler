cmake_minimum_required(VERSION 3.27)
project(compiler C)

set(CMAKE_C_STANDARD 11)

add_executable(compiler main.c
        libs/graph.c
        libs/graph.h
        tokenizer.c
        tokenizer.h
        memory_control.c
        memory_control.h
        libs/linked_list.c
        libs/linked_list.h
        parser.c
        parser.h
        libs/stack.c
        libs/stack.h
        libs/ht.h
        libs/ht.c
        generate_code.c
        generate_code.h
        error_handler.c
        error_handler.h
        asm_to_machine_code.c
        asm_to_machine_code.h
)
