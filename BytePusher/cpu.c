#include <stdlib.h>

#include "cpu.h"

void init_cpu(CPU* cpu, const uint8_t* rom, const size_t romSize)
{
	// address space is 24 bits (3 bytes), each address contains 8 bits (1 byte)
	size_t addressCount = 2;
	size_t addressSize = sizeof(uint8_t);

	for (size_t i = 0; i < 23; i++)
	{
		addressCount *= 2;
	}

	size_t space = addressCount * addressSize;
	uint8_t* memory = malloc(space);

	for (size_t i = 0; i < space; i++)
	{
		if (i < romSize) memory[i] = rom[i];
		else memory[i] = 0;
	}

	cpu->memory = memory;
	cpu->programCounter = fetch_address(cpu, 2);
}

void del_cpu(CPU* cpu)
{
	free(cpu->memory);
}

uint32_t fetch_address(CPU* cpu, const uint32_t index)
{
	// cpu is big-endian
	uint8_t* idx = cpu->memory + index;
	uint32_t addr = (idx[0] << 16) | (idx[1] << 8) | (idx[2]);
	return addr;
}

void tick(CPU* cpu)
{
	uint32_t pc = cpu->programCounter;

	uint32_t from = fetch_address(cpu, pc);
	uint32_t to = fetch_address(cpu, pc + 3);

	cpu->memory[to] = cpu->memory[from];
	cpu->programCounter = fetch_address(cpu, pc + 6);
}

void loop_cpu(CPU* cpu)
{
	cpu->programCounter = fetch_address(cpu, 2);

	for (size_t i = 0; i < 65536; i++)
	{
		tick(cpu);
	}
}
