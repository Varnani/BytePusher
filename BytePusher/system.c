#include <stdint.h>

#include <SDL.h>

#include "system.h"
#include "cpu.h"
#include "timestamp.h"

typedef struct Pixel {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} Pixel;

static CPU cpu;

static Pixel* palette;

static SDL_Renderer* screenRenderer;
static SDL_Event event;
static SDL_Texture* renderTarget;

TASK_RESULT do_frame()
{
	loop_cpu(&cpu);

	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT) return ABORT;
	}

	// pixel block is actually a 3 byte address to the 
	// beginning of the vm's frame buffer,
	// but only its leftmost byte is stored.
	// so when shifted by 16 bits to left, it points to the
	// first (x:0, y:0) pixel.
	uint32_t pixelBlock = (uint32_t)(cpu.memory[5]) << 16;

	Pixel* pixels = NULL;
	int pitch = 0;
	SDL_LockTexture(renderTarget, NULL, (void**)&pixels, &pitch);

	for (size_t y = 0; y < 256; y++)
	{
		for (size_t x = 0; x < 256; x++)
		{
			// texture pixel layout is RGB (3 bytes).
			// in theory, this should match our pixel struct
			Pixel* texturePixel = pixels + (x + y * 256);

			//first we figure out the pixel address inside virtual machine
			uint32_t pixelAddress = pixelBlock + (x + y * 256);

			//then we get the palette color from vm memory
			uint8_t paletteColor = cpu.memory[pixelAddress];

			//finally, we convert palette color to actual color
			*texturePixel = palette[paletteColor];
		}
	}

	SDL_UnlockTexture(renderTarget);

	SDL_RenderClear(screenRenderer);
	SDL_RenderCopy(screenRenderer, renderTarget, NULL, NULL);
	SDL_RenderPresent(screenRenderer);

	return CONTINUE;
}

void start_bytepusher(const uint8_t* rom, const size_t romSize, SDL_Renderer* renderer)
{
	screenRenderer = renderer;
	SDL_SetRenderDrawColor(screenRenderer, 255, 255, 255, 255);
	renderTarget = SDL_CreateTexture(screenRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 256, 256);

	// we map fixed palette colors to rgb values
	// later when we access this array by the palette color (8 bit uint),
	// we'll get a pixel struct with actual colors
	// only 216 colors are available,
	// so we'll walk each possible palette color and find out the rgb values 
	// and pack it inside a Pixel struct
	palette = malloc(256 * sizeof(Pixel));
	uint8_t intensity = 255 / 5;
	for (uint8_t i = 0; i < 255; i++)
	{
		Pixel* pix = palette + i;

		if (i < 216)
		{
			pix->red = ((i / 36) % 6) * intensity;
			pix->green = ((i / 6) % 6) * intensity;
			pix->blue = (i % 6) * intensity;
		}

		else
		{
			pix->red = 0;
			pix->green = 0;
			pix->blue = 0;
		}
	}

	init_cpu(&cpu, rom, romSize);

	start_task(&do_frame, 16667, SLEEP);

	del_cpu(&cpu);
}
