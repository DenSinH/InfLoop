add_library(PPU STATIC
        PPU.cpp
        PPU.h
        DistinctColors.h)

# for logging / stuff I want to use in all files
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/modules
        ../include
)

set_target_properties(PPU PROPERTIES LINKER_LANGUAGE CXX)


target_include_directories(PPU PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})