#include "init.h"
#include "sleeping.h"

#include "const.h"

#include <thread>

static Loopy* loopy;

void exception_handler() {
    // keep open the screen to still be able to check the register/memory contents (unstable)
    while (!loopy->Shutdown) {
        sleep_ms(16);
    }
}

#undef main
int main() {
    loopy = Initializer::init();

#ifdef DO_BREAKPOINTS
    loopy->Paused = false;
#endif

    atexit(exception_handler);

    std::thread ui_thread(ui_run);

    loopy->LoadROM("D:\\Data\\CProjects\\InfLoop\\files\\roms\\Animeland.bin");
    loopy->Run();

    ui_thread.join();

    return 0;
}
