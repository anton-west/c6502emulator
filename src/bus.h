#ifndef BUS_H
#define BUS_H

#include <stdint.h>

#define MAX_MEMORY 65536

typedef struct Bus {
    uint8_t memory[MAX_MEMORY] = {0};
} Bus;

#endif