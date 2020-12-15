find_package(SDL2 REQUIRED)

set(FRONTEND_SRCS
        imgui/imgui_impl_opengl3.cpp
        imgui/imgui_impl_sdl.cpp
        frontend.cpp
        widgets/menubar.cpp
        controller.cpp
        debugger.cpp
        settings.cpp
)

set(FRONTEND_HDRS
        imgui/imgui_impl_opengl3.h
        imgui/imgui_impl_sdl.h
        widgets/memory_viewer.h
        widgets/console.h
        widgets/menubar.h
        widgets/register_viewer.h
        widgets/disassembly_viewer.h
        widgets/overlay.h
        widgets/file_dialog.h
        debugger.h
        frontend.h
        interface.h
        controller.h
        settings.h
)

add_library(Frontend ${FRONTEND_SRCS} ${FRONTEND_HDRS})

set_target_properties(Frontend PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 20
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS ON)

target_link_libraries(Frontend PRIVATE ${SDL2_LIBRARY})

if (NOT WIN32)
    target_link_libraries(Frontend PRIVATE -ldl -lpthread)
endif()

target_include_directories(Frontend PRIVATE
        imgui
        ../../include
        ../../modules
        ${SDL2_INCLUDE_DIR}
)