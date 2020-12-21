#ifndef GC__SYSTEM_H
#define GC__SYSTEM_H

#include "SH7021/SH7021.h"
#include "Memory/Mem.h"

#include "default.h"
#include "flags.h"

#include "Scheduler/scheduler.h"
#include "Breakpoints/breakpoints.h"

class Loopy {

public:
    bool Paused = false;
    bool Shutdown = false;
    void (*Interaction)() = nullptr;

    Loopy();
    ~Loopy();
    void Run();
    void Reset();

    void LoadROM(const std::string& file_path) {
        Mem.LoadROM(file_path);
        Reset();
    }

    void ReloadROM() {

    }

    void LoadBIOS(std::string file_path) {

    }

private:
    friend class Initializer;

#ifdef DO_DEBUGGER
    u32 Stepcount = 0;

# ifdef DO_BREAKPOINTS
    s_breakpoints Breakpoints = {};
# endif

#endif

    i32 timer = 0;
    s_scheduler Scheduler = s_scheduler(&timer);

    Memory Mem = Memory(&Paused);
    SH7021 CPU = SH7021(&Mem, &timer);
};

#endif //GC__SYSTEM_H
