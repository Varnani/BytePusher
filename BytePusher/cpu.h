#pragma once

#include <stdint.h>

typedef struct CPU {
	uint32_t programCounter; // counter is actually 24 bits
	uint8_t* memory;
} CPU;

// init the cpu with provided rom
void init_cpu(CPU* cpu, const uint8_t* rom, const size_t romSize);

// deallocate cpu resources
void del_cpu(CPU* cpu);

// fetch a 24bit address from cpu memory at index
uint32_t fetch_address(CPU* cpu, const uint32_t index);

// loops the cpu, performing 65536 instructions
void loop_cpu(CPU* cpu);