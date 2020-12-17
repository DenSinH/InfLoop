add_library(SH7021 STATIC
        SH7021.h
        SH7021.cpp
        Interpreter.cpp
        Interpreter.h
        Tables.h
        Instruction.h
        Instructions/DataTransfer.cpp)

# for logging / stuff I want to use in all files
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/modules
        ${PROJECT_SOURCE_DIR}/src/Common
        ../include
)

set_target_properties(SH7021 PROPERTIES LINKER_LANGUAGE CXX)


target_include_directories(SH7021 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})