#include <cstdio>

#include "system.h"
#include "sleeping.h"
#include "const.h"

#include "flags.h"

Loopy::Loopy() {
#ifdef DO_BREAKPOINTS
    Breakpoints = {};
    Paused      = false;

    // add_breakpoint(&Breakpoints, 0x0000'0dac);
    // add_breakpoint(&Breakpoints, 0x0000'190e);
    // add_breakpoint(&Breakpoints, 0x0000'18d8);
    // add_breakpoint(&Breakpoints, 0x0800'c89e);
//    add_breakpoint(&Breakpoints, 0x0800'a5d2);
//    add_breakpoint(&Breakpoints, 0x0800'a5a6);
//    add_breakpoint(&Breakpoints, 0x0800'9702);
    // add_breakpoint(&Breakpoints, 0x00000f60);
    // add_breakpoint(&Breakpoints, 0x000000128);
    // add_breakpoint(&Breakpoints, 0x08000134);
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
                // CPU.Step();

//#if defined(DO_BREAKPOINTS) && defined(DO_DEBUGGER)
//                if (check_breakpoints(&Breakpoints, CPU.pc - ((CPU.CPSR & static_cast<u32>(CPSRFlags::T)) ? 4 : 8))) {
//                    log_debug("Hit breakpoint %08x", CPU.pc - ((CPU.CPSR & static_cast<u32>(CPSRFlags::T)) ? 4 : 8));
//                    Paused = true;
//                }
//#endif

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
                // todo: for getting started:
                Stepcount = 100;
                break;
            }

            // Scheduler.DoEvents();
            break; // todo: remove this
        }

        // todo: remove
//        Interaction();
//        Interaction = nullptr;
    }
}