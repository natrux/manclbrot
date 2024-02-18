#include <string>
#include <stdexcept>

#include <SDL2/SDL.h>


// Squares the complex number c = (x + y*i) and returns |c|^2.
static double square(double &x, double &y){
	const double x_sq = x * x;
	const double y_sq = y * y;

	y = 2 * x * y;
	x = x_sq - y_sq;
	return x_sq + y_sq;
}


// Performs limit many iterations to test if c = (x_0 + y_0 * i) is likely in the Mandelbrot Set.
// Returns 0 if yes, otherwise the smallest n such that |z_n| > 2 (providing a witness for the opposite).
static unsigned long mandelbrot_test(double x_0, double y_0, unsigned long limit){
	double x = x_0;
	double y = y_0;

	for(unsigned long i=1; i<limit; i++){
		const double abs_sq = square(x, y);
		if(abs_sq > 4){
			return i;
		}
		x += x_0;
		y += y_0;
	}
	return 0;
}



static void writepixel(void *pixels, const SDL_PixelFormat *format, int pitch, int x, int y, uint32_t color){
	char *src = reinterpret_cast<char *>(&color) + (sizeof(color) - format->BytesPerPixel);
	char *dst = reinterpret_cast<char *>(pixels) + y*pitch + x*format->BytesPerPixel;
	memcpy(dst, src, format->BytesPerPixel);
}

static void writepixel_rgb(void *pixels, const SDL_PixelFormat *format, int pitch, int x, int y, uint8_t r, uint8_t g, uint8_t b){
	const uint32_t rgb = SDL_MapRGBA(format, r, g, b, 255);
        writepixel(pixels, format, pitch, x, y, rgb);
}


int main(int /*argc*/, char **/*argv*/){
	const int screen_width = 640;
	const int screen_height = 480;
	double zoom = 0.005;
	double offset_x = screen_width * zoom * 2 / 3;
	double offset_y = screen_height * zoom / 2;

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		throw std::runtime_error("SDL_Init() failed with: " + std::string(SDL_GetError()));
	}

	SDL_Window* window = SDL_CreateWindow("manCLbrot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
	if(!window){
		throw std::runtime_error("SDL_CreateWindow() failed with: " + std::string(SDL_GetError()));
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(!renderer){
		throw std::runtime_error("SDL_CreateRenderer() failed with: " + std::string(SDL_GetError()));
	}
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
	if(!texture){
		throw std::runtime_error("SDL_CreateTexture() failed with: " + std::string(SDL_GetError()));
	}

	SDL_Rect draw_area;
	draw_area.x = 0;
	draw_area.y = 0;
	uint32_t pixel_format;
	if(SDL_QueryTexture(texture, &pixel_format, NULL, &draw_area.w, &draw_area.h) < 0){
		throw std::runtime_error("SDL_QueryTexture() failed with: " + std::string(SDL_GetError()));
	}
	SDL_PixelFormat *format = SDL_AllocFormat(pixel_format);
	if(!format){
		throw std::runtime_error("SDL_AllocFormat() failed with: " + std::string(SDL_GetError()));
	}


	void *pixels;
	int pitch;
	if(SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0){
		throw std::runtime_error("SDL_LockTexture() failed with: " + std::string(SDL_GetError()));
	}
	for(int x=0; x<screen_width; x++){
		for(int y=0; y<screen_height; y++){
			const auto test = mandelbrot_test(x*zoom-offset_x, y*zoom-offset_y, 100);
			if(test == 0){
				writepixel_rgb(pixels, format, pitch, x, y, 0, 0, 0);
			}else{
				writepixel_rgb(pixels, format, pitch, x, y, 255, 255, 255);
			}
		}
	}
	SDL_UnlockTexture(texture);

	if(SDL_RenderClear(renderer) < 0){
		throw std::runtime_error("SDL_RenderClear() failed with: " + std::string(SDL_GetError()));
	}

	if(SDL_RenderCopy(renderer, texture, NULL, &draw_area) < 0){
		throw std::runtime_error("SDL_RenderCopy() failed with: " + std::string(SDL_GetError()));
	}

	SDL_RenderPresent(renderer);

	SDL_Delay(5000);

	SDL_FreeFormat(format);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}


