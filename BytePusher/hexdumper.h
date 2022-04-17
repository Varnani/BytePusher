#pragma once

#include <stdlib.h>

static const int BYTE_PER_LINE = 16;

char* hexdump(const void* buffer, const size_t bufferSize);