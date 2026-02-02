cmake_minimum_required(VERSION 4.0)
project(BLSLang)

set(CMAKE_CXX_STANDARD 26)

add_executable(BLSLang src/main.cpp

        # Headers
        src/headers/core/blsl.h
        src/headers/core/blsbyc.h
        src/headers/log/log.h
        src/headers/intermediate/lexer.h
        src/headers/runtime/operator.h
        src/headers/intermediate/parser.h
        src/headers/runtime/vm.h
        src/headers/runtime/mem.h
        src/headers/runtime/arena.h
        src/headers/err/err.h
        src/headers/intermediate/astnodes.h
        src/headers/intermediate/astnodes.h
        src/headers/intermediate/compiler.h

        # Implementations
        src/implementations/core/core.cpp
        src/implementations/intermediate/lexer.cpp
        src/implementations/intermediate/parser.cpp
        src/implementations/intermediate/compiler.cpp
        src/implementations/log/log.cpp
        src/implementations/runtime/vm.cpp
        src/implementations/runtime/arena.cpp
        src/implementations/runtime/operator.cpp
        src/implementations/intermediate/astnodes.cpp
        src/implementations/err/err.cpp
        src/implementations/runtime/mem.cpp
        src/implementations/debug/debug.cpp
        src/headers/debug/debug.h
        src/headers/runtime/bootloader.h
        src/implementations/runtime/bootloader.cpp


)