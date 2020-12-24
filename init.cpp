#include "init.h"

#include "default.h"
#include "debugging.h"

#include <memory>

static Loopy* loopy;

CONSOLE_COMMAND(Initializer::reset_system) {
#ifdef DO_DEBUGGER
    if (argc > 1 && (
            strcmp(args[1], "freeze") == 0 ||
            strcmp(args[1], "pause")  == 0 ||
            strcmp(args[1], "break")  == 0
    )
            ) {
        loopy->Paused = true;
    }
#endif

    loopy->Interaction = [] {
//        loopy->Reset();
//        loopy->ReloadROM();
    };
}

CONSOLE_COMMAND(Initializer::pause_system) {
#ifdef DO_DEBUGGER
    loopy->Paused = true;

    STRCPY(output, MAX_OUTPUT_LENGTH, "System paused");
#endif
}

CONSOLE_COMMAND(Initializer::unpause_system) {
#ifdef DO_DEBUGGER
    loopy->Paused = false;

    STRCPY(output, MAX_OUTPUT_LENGTH, "System unpaused");
#endif
}

CONSOLE_COMMAND(Initializer::break_system) {
#ifdef DO_DEBUGGER
    if (argc < 2) {
        pause_system(args, argc, output);
        return;
    }

#ifdef DO_BREAKPOINTS
    u32 breakpoint = parsehex(args[1]);
    add_breakpoint(&loopy->Breakpoints, breakpoint);
    SPRINTF(output, MAX_OUTPUT_LENGTH, "Added breakpoint at %08x", breakpoint);
#else
    STRCPY(output, MAX_OUTPUT_LENGTH, "Breakpoints disabled");
#endif
#endif
}

CONSOLE_COMMAND(Initializer::unbreak_system) {
#ifdef DO_DEBUGGER
    if (argc < 2) {
        unpause_system(args, argc, output);
        return;
    }
#ifdef DO_BREAKPOINTS
    u32 breakpoint = parsehex(args[1]);
    remove_breakpoint(&loopy->Breakpoints, breakpoint);
    SPRINTF(output, MAX_OUTPUT_LENGTH, "Removed breakpoint at %08x", breakpoint);
#else
    STRCPY(output, MAX_OUTPUT_LENGTH, "Breakpoints disabled");
#endif
#endif
}

CONSOLE_COMMAND(Initializer::step_system) {
#ifdef DO_DEBUGGER
    if (argc < 2) {
        loopy->Paused = true;
        loopy->Stepcount = 1;
        STRCPY(output, MAX_OUTPUT_LENGTH, "Stepping system for one step");
    } else {
        loopy->Paused = true;
        u32 steps = parsedec(args[1]);
        loopy->Stepcount = steps;
        SPRINTF(output, MAX_OUTPUT_LENGTH, "Stepping system for %d steps", steps);
    }
#endif
}

CONSOLE_COMMAND(Initializer::trace_system) {
#ifdef DO_DEBUGGER
    if (argc < 2) {
        STRCPY(output, MAX_OUTPUT_LENGTH, "Please give a number of steps");
    } else {
#ifdef TRACE_LOG
        u32 steps = parsedec(args[1]);
        loopy->CPU.TraceSteps = steps;
        SPRINTF(output, MAX_OUTPUT_LENGTH, "Tracing system for %d steps", steps);
#else
        STRCPY(output, MAX_OUTPUT_LENGTH, "Trace logging is turned off");
#endif
    }
#endif
}

static u64 ticks, prev_ticks;
OVERLAY_INFO(Initializer::cpu_ticks) {
    ticks = loopy->timer;
    SPRINTF(output, output_length, "CPU ticks/s: %.1f", (float)(ticks - prev_ticks) / delta_time);
    prev_ticks = ticks;
}

static float accum_time;
static size_t prev_frame;
OVERLAY_INFO(Initializer::fps_counter) {
    SPRINTF(
            output,
            output_length,
            "N/A"
    );
//    accum_time += delta_time;
//    SPRINTF(
//            output,
//            output_length,
//                "FPS        : %.1f\n"
//                "Frame time : %.3fms",
//            (double)(loopy->PPU.Frame) / accum_time,
//            1000.0 * delta_time / (float)(loopy->PPU.Frame - prev_frame)
//    );
//    if (accum_time > 1) {
//        loopy->PPU.Frame = 0;
//        accum_time = 0;
//    }
//    prev_frame = loopy->PPU.Frame;
}

OVERLAY_INFO(Initializer::scheduler_events) {
    SPRINTF(
            output,
            output_length,
            "Scheduler  : %d events",
            loopy->Scheduler.queue.size()
    );
}

OVERLAY_INFO(Initializer::scheduler_time_until) {
    SPRINTF(
            output,
            output_length,
            "Sched. next: %d cycles",
            loopy->Scheduler.top - *loopy->Scheduler.timer
    );
}

OVERLAY_INFO(Initializer::audio_samples) {
    SPRINTF(
            output,
            output_length,
            "N/A"
    );
//    if (loopy->APU.Stream) {
//        SPRINTF(
//                output,
//                output_length,
//                "Samples    : %d bytes",
//                SDL_AudioStreamAvailable(loopy->APU.Stream)
//        );
//    }
//    else {
//        STRCPY(
//                output,
//                output_length,
//                "NO STREAM AVAILABLE"
//        );
//    }
}

static std::string file;
FILE_SELECT_CALLBACK(load_ROM_callback) {
    file = (std::string)file_name;
    loopy->Interaction = []{ loopy->LoadROM(file); };
}

MENU_ITEM_CALLBACK(load_ROM) {
    const char* filters[2] = {".gba", ""};
    open_file_explorer("Select ROM file", const_cast<char **>(filters), 2, load_ROM_callback);
}

u8 Initializer::ReadByte(u64 offset) {
    return loopy->Mem.Read<u8, true>(offset);
}

u16 Initializer::ReadInstr(u32 address) {
    return loopy->Mem.Read<u16, true>(address);
}

void Initializer::ParseInput(struct s_controller* controller) {
//    u16 KEYINPUT = 0;
//    if (controller->A | controller->X) KEYINPUT |= static_cast<u16>(KeypadButton::A);
//    if (controller->B | controller->Y) KEYINPUT |= static_cast<u16>(KeypadButton::B);
//    if (controller->start)             KEYINPUT |= static_cast<u16>(KeypadButton::Start);
//    if (controller->select)            KEYINPUT |= static_cast<u16>(KeypadButton::Select);
//    if (controller->left)              KEYINPUT |= static_cast<u16>(KeypadButton::Left);
//    if (controller->right)             KEYINPUT |= static_cast<u16>(KeypadButton::Right);
//    if (controller->down)              KEYINPUT |= static_cast<u16>(KeypadButton::Down);
//    if (controller->up)                KEYINPUT |= static_cast<u16>(KeypadButton::Up);
//    if (controller->L)                 KEYINPUT |= static_cast<u16>(KeypadButton::L);
//    if (controller->R)                 KEYINPUT |= static_cast<u16>(KeypadButton::R);

    // bits are flipped
    // loopy->IO.KEYINPUT = ~KEYINPUT;
}

void Initializer::frontend_video_init() {
    loopy->PPU.VideoInit();
}

s_framebuffer Initializer::frontend_render(size_t t) {
    // return loopy->PPU.RenderUntil(t);
    return {};
}

void Initializer::frontend_blit(const float* data) {
    // loopy->PPU.Blit(data);
}

void Initializer::frontend_video_destroy() {
    // loopy->PPU.VideoDestroy();
}

void Initializer::frontend_audio_init() {
    // loopy->APU.AudioInit();
}

void Initializer::frontend_audio_destroy() {
    // loopy->APU.AudioDestroy();
}

Loopy* Initializer::init() {
    loopy = new Loopy;

    bind_video_init(frontend_video_init);
    bind_video_render(frontend_render);
    bind_video_blit(frontend_blit);
    bind_video_destroy(frontend_video_destroy);

    bind_audio_init(frontend_audio_init);
    bind_audio_destroy(frontend_audio_destroy);

    frontend_init(
            []{ loopy->Shutdown = true; loopy->Interaction = []{}; },  // need to set interaction function to break it out of the loop
            ParseInput
    );

    debugger_init(
            &loopy->CPU.PC,
            0x1'0000'0000ULL,
            [](u32 address) { return loopy->Mem.Read<u16, true>(address); },
            [](u64 offset) { return loopy->Mem.Read<u8, true>(offset); },
            [](int index){ return loopy->Mem.ReadPalletteEntry(index); }
    );

    char label[0x100];

    int cpu_tab = add_register_tab("SH7021");

    for (int i = 0; i < 16; i++) {
        sprintf(label, "r%d", i);
        add_register_data(label, &loopy->CPU.R[i], 4, cpu_tab);
    }

    add_register_data("PC", &loopy->CPU.PC, 4, cpu_tab);
    add_register_data("PR", &loopy->CPU.PR, 4, cpu_tab);
    add_register_data("SR", &loopy->CPU.SR, 4, cpu_tab);
    add_register_data("GBR", &loopy->CPU.GBR, 4, cpu_tab);
    add_register_data("VBR", &loopy->CPU.VBR, 4, cpu_tab);
    add_register_data("MACL", &loopy->CPU.MACL, 4, cpu_tab);
    add_register_data("MACH", &loopy->CPU.MACH, 4, cpu_tab);

    int vreg_tab = add_register_tab("Video");

    add_register_data("c058004", &loopy->Mem.IOVideoInterface.VTR, 2, vreg_tab);

    add_image_window(
        "Tiles",
        [](){ return loopy->PPU.DrawTileData(); },
        true,
        256,
        256,
        // scale * 2
        512,
        480 * 2,
        [](bool left) { loopy->PPU.OnTileDataClick(left); }
    );

    add_image_window("Tilemap 0",
        [](){ return loopy->PPU.DrawTileMap(0); },true,
        256,224,
        256,224,
        [](bool left) { loopy->PPU.OnTileMapClick(left); }
    );

    add_image_window("Tilemap 1",
        [](){ return loopy->PPU.DrawTileMap(1); }, true,
        256,224,
                     256,224,
        [](bool left) { loopy->PPU.OnTileMapClick(left); }
    );


    add_command("RESET", "Resets the system. Add 'pause/freeze/break' to freeze on reload.", reset_system);
    add_command("PAUSE", "Pauses the system.", pause_system);
    add_command("CONTINUE", "Unpauses the system.", unpause_system);
    add_command("BREAK", "Add breakpoint to system at PC = $1.", break_system);
    add_command("UNBREAK", "Remove breakpoint to system at PC = $1.", unbreak_system);
    add_command("STEP", "Step system for $1 CPU steps (defaults to 1 step).", step_system);
    add_command("TRACE", "Trace system for $1 CPU steps.", trace_system);

    add_overlay_info(cpu_ticks);
    add_overlay_info(fps_counter);
    add_overlay_info(scheduler_events);
    add_overlay_info(scheduler_time_until);
    add_overlay_info(audio_samples);

    return loopy;
}