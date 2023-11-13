#include "pico/multicore.h"

void run_paralel()
{
    uint32_t data = 44;

    multicore_fifo_push_blocking(data);

    uint32_t r = multicore_fifo_pop_blocking();
}
