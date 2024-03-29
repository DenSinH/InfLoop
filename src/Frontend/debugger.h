#pragma once

#include <glad/glad.h>
#include <SDL.h>

#undef SHOW_EXAMPLE_MENU

void debugger_video_init(const char* glsl_version, SDL_Window* window, SDL_GLContext* gl_context);
void debugger_render();