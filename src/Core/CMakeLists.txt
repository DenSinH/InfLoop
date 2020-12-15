add_library(Core STATIC
        system.h
        system.cpp
        Breakpoints/breakpoints.h
        Breakpoints/breakpoints.c
        Scheduler/scheduler.h
        CoreUtils.cpp
        include/CoreUtils.h
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        # $<TARGET_PROPERTY:ARM7TDMI,INCLUDE_DIRECTORIES>
)

set_target_properties(Core PROPERTIES LINKER_LANGUAGE CXX)

# target_link_libraries(Core ARM7TDMI)

target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})