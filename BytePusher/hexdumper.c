#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "hexdumper.h"

char* hexdump(const void* buffer, const size_t bufferSize)
{
	size_t dumpIndex = 0;
	size_t i = 0;

	uint8_t* dataBuffer = (uint8_t*)buffer;
	char* asciiBuffer = malloc((BYTE_PER_LINE + 1) * sizeof(char));

	//             -offset     -per byte     -padding  -ascii    -newline
	int charPerLine = 12 + (3 * BYTE_PER_LINE) + 5 + BYTE_PER_LINE + 1;
	int totalLines = bufferSize / BYTE_PER_LINE;
	int totalChars = charPerLine * totalLines;

	char* textDump = malloc(totalChars * sizeof(char));
	for (size_t index = 0; index < totalChars; index++)
	{
		textDump[index] = 0;
	}
	textDump[totalChars - 1] = '\0';

	for (; i < bufferSize; i++)
	{
		int modulo = i % BYTE_PER_LINE;

		if (modulo == 0)
		{
			if (i != 0)
			{
				dumpIndex += sprintf_s(textDump + dumpIndex, totalChars - dumpIndex, "  |  %s\n", asciiBuffer);
			}

			dumpIndex += sprintf_s(textDump + dumpIndex, totalChars - dumpIndex, " %08x ", i);
		}

		uint8_t byte = dataBuffer[i];
		dumpIndex += sprintf_s(textDump + dumpIndex, totalChars - dumpIndex, " %02x", byte);

		if (isprint(byte)) asciiBuffer[modulo] = byte;
		else asciiBuffer[modulo] = '.';
		asciiBuffer[modulo + 1] = '\0';
	}

	while (i % BYTE_PER_LINE > 0)
	{
		dumpIndex += sprintf_s(textDump + dumpIndex, totalChars - dumpIndex, "   ");
		i++;
	}

	sprintf_s(textDump + dumpIndex, totalChars - dumpIndex, "  |  %s\n", asciiBuffer);

	free(asciiBuffer);
	return textDump;
}