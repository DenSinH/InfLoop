add_library(Core STATIC
        system.h
        system.cpp
        Breakpoints/breakpoints.h
        Breakpoints/breakpoints.c
        Scheduler/scheduler.h
        CoreUtils.cpp
        include/CoreUtils.h
)

add_subdirectory(SH7021)
add_subdirectory(Memory)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $<TARGET_PROPERTY:SH7021,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Mem,INCLUDE_DIRECTORIES>
)

set_target_properties(Core PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(Core SH7021)
target_link_libraries(Core Mem)

target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})