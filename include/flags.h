#ifndef GC__FLAGS_H
#define GC__FLAGS_H

#define VERBOSITY_ALL 0
#define VERBOSITY_DEBUG 1
#define VERBOSITY_INFO 2
#define VERBOSITY_WARN 3
#define VERBOSITY_ERROR 4
#define VERBOSITY_MAX VERBOSITY_ERROR

#define COMPONENT_CPU_VERBOSE 0x01
#define COMPONENT_SCHEDULER   0x02
#define COMPONENT_CPU         0x04
#define COMPONENT_PPU         0x08
#define COMPONENT_IO          0x10
#define COMPONENT_DMA         0x20
#define COMPONENT_TIMERS      0x40
#define COMPONENT_MEMORY      0x80

#ifndef NDEBUG
// change to change verbosity / component logging:
#define VERBOSITY VERBOSITY_DEBUG
#define COMPONENT_FLAGS (COMPONENT_IO)

// checks
#define DO_DEBUGGER
#define DO_BREAKPOINTS

// optimizations
#define DIRTY_MEMORY_ACCESS

#else
#define VERBOSITY VERBOSITY_MAX

#define DIRTY_MEMORY_ACCESS

#endif

#endif //GC__FLAGS_H
