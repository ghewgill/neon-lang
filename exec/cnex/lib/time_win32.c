#include <iso646.h>
#include <stdio.h>
#include <windows.h>

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"
#include "time.h"


static const ULONGLONG FILETIME_UNIX_EPOCH = 116444736000000000ULL;
static Number PERFORMANCE_FREQUENCY;

void time_initModule()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    PERFORMANCE_FREQUENCY = number_from_uint64(freq.QuadPart);
}

void time_sleep(TExecutor *exec)
{
    Number seconds = top(exec->stack)->number; pop(exec->stack);

    // ToDo: If the number of seconds is greater than MAX_INT / 1000, then call Sleep() multiple times.
    if (number_is_greater(seconds, number_from_uint32(4294967))) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Requested time to sleep exceeds 4294967: %s", number_to_string(seconds));
        exec->rtl_raise(exec, "PANIC", buf);
        return;
    }
    uint32_t ms = number_to_uint32(number_multiply(seconds, number_from_uint32(1000)));
    Sleep(ms);
}

void time_now(TExecutor *exec)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ticks;
    ticks.LowPart = ft.dwLowDateTime;
    ticks.HighPart = ft.dwHighDateTime;
    push(exec->stack, cell_fromNumber(number_divide(number_from_uint64(ticks.QuadPart - FILETIME_UNIX_EPOCH), number_from_uint32(10000000))));
}

void time_tick(TExecutor *exec)
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    push(exec->stack, cell_fromNumber(number_divide(number_from_uint64(now.QuadPart), PERFORMANCE_FREQUENCY)));
}

