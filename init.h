#pragma once

#include "src/Frontend/interface.h"
#include "src/Core/system.h"

#include "default.h"

class Initializer {
public:
    static Loopy* init();

private:
    static CONSOLE_COMMAND(reset_system);
    static CONSOLE_COMMAND(step_system);
    static CONSOLE_COMMAND(trace_system);
    static CONSOLE_COMMAND(break_system);
    static CONSOLE_COMMAND(unbreak_system);
    static CONSOLE_COMMAND(pause_system);
    static CONSOLE_COMMAND(unpause_system);

    static OVERLAY_INFO(cpu_ticks);
    static OVERLAY_INFO(fps_counter);
    static OVERLAY_INFO(scheduler_events);
    static OVERLAY_INFO(scheduler_time_until);
    static OVERLAY_INFO(audio_samples);

    static u8 ReadByte(u64 offset);
    static u16 ReadInstr(u32 address);
    static void ParseInput(struct s_controller* controller);

    static void frontend_video_init();
    static s_framebuffer frontend_render(size_t);
    static void frontend_blit(const float* data);
    static void frontend_video_destroy();

    static void frontend_audio_init();
    static void frontend_audio_destroy();
};
