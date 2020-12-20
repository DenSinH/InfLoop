add_library(SH7021 STATIC
        SH7021.h
        SH7021.cpp
        Tables.h
        Instruction.h
        Tables.h
        Tables.cpp
        Instructions/DataTransfer.cpp
        Instructions/Arithmetic.cpp
        Instructions/Branch.cpp Instructions/Logic.cpp Instructions/Branch.inl Instructions/Arithmetic.inl Instructions/Shift.inl BIOS.cpp)

# for logging / stuff I want to use in all files
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/modules
        ${PROJECT_SOURCE_DIR}/src/Common
        ../include
)

set_target_properties(SH7021 PROPERTIES LINKER_LANGUAGE CXX)


target_include_directories(SH7021 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})