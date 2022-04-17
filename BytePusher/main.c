#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "hexdumper.h"
#include "system.h"

int main(int argc, char** argv[])
{
	char* filename;
	if (argc >= 2) filename = argv[1];
	else { printf("err: filename required.\n"); return 1; }

	FILE *file;
	errno_t err = fopen_s(&file, filename, "rb");

	if (err != 0)
	{
		if (err == 2) printf("err: can't find file.");
		else printf("err: file open error: %d\n", err);

		return 1;
	}

	printf("opening rom: %s\n", filename);

	fseek(file, 0, SEEK_END);
	uint64_t fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	printf("romsize in bytes: %d\n", (int)fileSize);
	printf("romsize in kib  : %d\n", (int)fileSize / 1024);

	size_t bufferSize = sizeof(uint8_t) * fileSize;
	uint8_t* buffer = malloc(bufferSize);

	if (buffer == 0)
	{
		printf("err: can't allocate memory for the rom.\n");
		return 1;
	}

	fread(buffer, sizeof(uint8_t), bufferSize, file);

	char* dump = hexdump(buffer, 64);
	printf("first 64 bytes of the rom:\n%s\n", dump);
	free(dump);

	printf("starting SDL...\n");

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) printf("SDL init error. SDL_Error: %s\n", SDL_GetError());

	else
	{
		window = SDL_CreateWindow("BytePusher", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_SHOWN);
		if (window == NULL) printf("can't create window. SDL_Error: %s\n", SDL_GetError());

		else
		{
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			start_bytepusher(buffer, bufferSize, renderer);
		}
	}

	printf("quitting...\n");

	SDL_DestroyWindow(window);
	SDL_Quit();

	free(buffer);
}