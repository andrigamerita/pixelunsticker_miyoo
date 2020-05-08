#include <SDL/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <random>

static bool running = true;

void handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_RCTRL:
						running = false;
						break;
					case SDLK_RETURN:
						running = false;
						break;
					case SDLK_ESCAPE:
						running = false;
						break;
					default:
						break;
				}
				break;
			case SDL_QUIT:
				running = false;
				break;
			default:
				break;
		}
	}
}

static std::ranlux24_base rng;

void paint(SDL_Surface *surface)
{
	typedef uint32_t Pixel;
	const SDL_PixelFormat *format = surface->format;
	Pixel colors[8] = {
		SDL_MapRGB(format,   0,   0,   0),
		SDL_MapRGB(format,   0,   0, 255),
		SDL_MapRGB(format,   0, 255,   0),
		SDL_MapRGB(format,   0, 255, 255),
		SDL_MapRGB(format, 255,   0,   0),
		SDL_MapRGB(format, 255,   0, 255),
		SDL_MapRGB(format, 255, 255,   0),
		SDL_MapRGB(format, 255, 255, 255),
	};

	const int width = surface->w, height = surface->h;

	Pixel *line = static_cast<Pixel *>(surface->pixels);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x += 8) {
			uint32_t pattern = rng();
			line[x + 0] = colors[(pattern >>  0) & 7];
			line[x + 1] = colors[(pattern >>  3) & 7];
			line[x + 2] = colors[(pattern >>  6) & 7];
			line[x + 3] = colors[(pattern >>  9) & 7];
			line[x + 4] = colors[(pattern >> 12) & 7];
			line[x + 5] = colors[(pattern >> 15) & 7];
			line[x + 6] = colors[(pattern >> 18) & 7];
			line[x + 7] = colors[(pattern >> 21) & 7];
		}
		line = reinterpret_cast<Pixel *>(
				reinterpret_cast<uint8_t *>(line) + surface->pitch);
	}
}

int main(int argc, char **argv)
{
	(void)argv; (void)argc;

	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);

	SDL_ShowCursor(SDL_DISABLE);
	SDL_Surface *screen = SDL_SetVideoMode(
			320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (screen->format->BitsPerPixel != 32) {
		fprintf(stderr, "ERROR: Did not get 32 bpp, got %u bpp instead.\n",
				screen->format->BitsPerPixel);
		exit(1);
	}
	if (!(screen->flags & SDL_HWSURFACE)) {
		fprintf(stderr, "WARNING: Did not get a hardware surface.\n");
	}
	if (!(screen->flags & SDL_DOUBLEBUF)) {
		fprintf(stderr, "WARNING: Did not get double buffering.\n");
	}

	Uint32 start = SDL_GetTicks();
	Uint32 lastPrint = start;
	int frameCount = 0;
	while (running) {
		SDL_LockSurface(screen);
		paint(screen);
		SDL_UnlockSurface(screen);
		SDL_Flip(screen);
		frameCount++;

		Uint32 now = SDL_GetTicks();
		if (now - lastPrint >= 1000) {
			float fps = frameCount / (0.001f * (now - lastPrint));
			printf("%3.2f frames per second\n", fps);
			lastPrint = now;
			frameCount = 0;
		}

		handleEvents();
	}

	return 0;
}
