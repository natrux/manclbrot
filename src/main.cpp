#include <string>
#include <vector>
#include <chrono>
#include <random>
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



static void writepixel(void *pixels, uint8_t bytes_per_pixel, int pitch, int x, int y, uint32_t color){
	char *src = reinterpret_cast<char *>(&color) + (sizeof(color) - bytes_per_pixel);
	char *dst = reinterpret_cast<char *>(pixels) + y*pitch + x*bytes_per_pixel;
	memcpy(dst, src, bytes_per_pixel);
}

static void writepixel_rgb(void *pixels, const SDL_PixelFormat *format, int pitch, int x, int y, uint8_t r, uint8_t g, uint8_t b){
	const uint32_t rgb = SDL_MapRGBA(format, r, g, b, 255);
        writepixel(pixels, format->BytesPerPixel, pitch, x, y, rgb);
}


static std::vector<uint32_t> make_random_colors(size_t count, const SDL_PixelFormat *format){
	std::random_device device;
	std::mt19937_64 generator(device());
	const auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	const auto seed = static_cast<decltype(generator)::result_type>(time);
	generator.seed(seed);

	int16_t r = 0, g = 0, b = 0;
	int8_t dr = 0, dg = 0, db = 0;
	const uint8_t min_delta = 1;
	const uint8_t max_delta = 20;
	std::uniform_int_distribution<int8_t> distribution(min_delta, max_delta);

	std::vector<uint32_t> result;
	result.resize(count);
	result[0] = SDL_MapRGBA(format, r, g, b, 255);
	for(size_t i=1; i<count; i++){
		if(r == 0 || r == 255){
			const int8_t d = distribution(generator);
			dr = (r == 0) ? d : -d;
		}
		if(g == 0 || g == 255){
			const int8_t d = distribution(generator);
			dg = (g == 0) ? d : -d;
		}
		if(b == 0 || b == 255){
			const int8_t d = distribution(generator);
			db = (b == 0) ? d : -d;
		}

		r = std::min(std::max(r+dr, 0), 255);
		g = std::min(std::max(g+dg, 0), 255);
		b = std::min(std::max(b+db, 0), 255);

		result[i] = SDL_MapRGBA(format, r, g, b, 255);
	}
	return result;
}



int main(int /*argc*/, char **/*argv*/){
	const int screen_width = 640;
	const int screen_height = 480;
	double zoom = 0.005;
	double offset_x = screen_width * zoom * 2 / 3;
	double offset_y = screen_height * zoom / 2;
	const unsigned long iter_limit = 100;

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
	const auto colors = make_random_colors(iter_limit, format);


	void *pixels;
	int pitch;
	if(SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0){
		throw std::runtime_error("SDL_LockTexture() failed with: " + std::string(SDL_GetError()));
	}
	for(int x=0; x<screen_width; x++){
		for(int y=0; y<screen_height; y++){
			const auto test = mandelbrot_test(x*zoom-offset_x, y*zoom-offset_y, iter_limit);
			writepixel(pixels, format->BytesPerPixel, pitch, x, y, colors[test]);
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


