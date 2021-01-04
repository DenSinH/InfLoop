#include <cstdio>

#include "system.h"
#include "sleeping.h"
#include "const.h"

#include "flags.h"

Loopy::Loopy() {
#ifdef DO_BREAKPOINTS
    Breakpoints = {};
    Paused      = false;

    // add_breakpoint(&Breakpoints, 0x0e003b50);
    // add_breakpoint(&Breakpoints, 0x0e0126c0);
//    add_breakpoint(&Breakpoints, 0x0e0109b2);
//    add_breakpoint(&Breakpoints, 0x0e0109b8);
//     add_breakpoint(&Breakpoints, 0x0e0135da);
//     add_breakpoint(&Breakpoints, 0x0e0135e4);
//     add_breakpoint(&Breakpoints, 0x0e0135ea);
//     add_breakpoint(&Breakpoints, 0x0e0135f0);
//     add_breakpoint(&Breakpoints, 0x0e0135fa);
//      Loop that animeland breaks in:
//    add_breakpoint(&Breakpoints, 0x0e013aee);
//    add_breakpoint(&Breakpoints, 0x0e013aca);

//    add_breakpoint(&Breakpoints, 0x0e014134);  // int r0 to double
//    add_breakpoint(&Breakpoints, 0x0e01413a);  // return

    add_breakpoint(&Breakpoints, 0x0e00fbb8);  // copying girl data animeland
#endif
}

Loopy::~Loopy() {

}

void Loopy::Reset() {

}

void Loopy::Run() {
    // CPU.SkipBIOS();

    while (!Shutdown) {

        while (!Interaction) {
            while (likely(!Scheduler.ShouldDoEvents())) {
                CPU.Step();

#if defined(DO_BREAKPOINTS) && defined(DO_DEBUGGER)
                if (check_breakpoints(&Breakpoints, CPU.PC - 2)) {
                    log_debug("Hit breakpoint %08x", CPU.PC - 2);
                    Paused = true;
                }
#endif

#ifdef DO_DEBUGGER
                while (Paused && (Stepcount == 0) && !Shutdown) {
                    // sleep for a bit to not have a busy wait. This is for debugging anyway, and we are frozen
                    // so it's okay if it's not instant
                    sleep_ms(16);
                }

                if (Stepcount > 0) {
                    Stepcount--;
                }
#endif
                // todo: until we get events in the scheduler
                if (Shutdown) {
                    return;
                }
            }

            Scheduler.DoEvents();
        }

        Interaction();
        Interaction = nullptr;
    }
}