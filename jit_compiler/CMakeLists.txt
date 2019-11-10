cmake_minimum_required(VERSION 3.12)
project(jit_compiler_lib)

set(CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/sysroot-glibc-linaro)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-marm ${CMAKE_CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR})
set(SOURCE_LIB tokenizer.cpp translator_into_rpn.cpp arm32_instructions_encoder.cpp jit_compiler_into_arm32.cpp
        tokens/addressed_rpn_token.cpp tokens/function_rpn_token.cpp tokens/variable_rpn_token.cpp
        tokens/literal_rpn_token.cpp tokens/minus_rpn_token.cpp tokens/mult_rpn_token.cpp tokens/plus_rpn_token.cpp tokens/tokens.h)
add_library(jit_compiler_lib SHARED ${SOURCE_LIB})
