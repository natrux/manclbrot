#include <manclbrot/MandelbrotRenderer.h>

#include <string>
#include <stdexcept>
#include <memory>

#include <SDL2/SDL.h>



int main(int /*argc*/, char **/*argv*/){
	const int screen_width = 640;
	const int screen_height = 480;
	double zoom = 0.005;
	double offset_x = screen_width * zoom * 2 / 3;
	double offset_y = screen_height * zoom / 2;
	const unsigned long iter_limit = 100;
	const bool use_opencl = false;

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
	if(SDL_QueryTexture(texture, NULL, NULL, &draw_area.w, &draw_area.h) < 0){
		throw std::runtime_error("SDL_QueryTexture() failed with: " + std::string(SDL_GetError()));
	}


	std::unique_ptr<MandelbrotRendererInterface> mandelbrot_renderer;
	if(use_opencl){
		throw std::logic_error("Not implemented");
	}else{
		mandelbrot_renderer = std::make_unique<MandelbrotRenderer>(screen_width, screen_height, texture);
	}
	mandelbrot_renderer->set_iter_limit(iter_limit);
	mandelbrot_renderer->draw(zoom, offset_x, offset_y);


	if(SDL_RenderClear(renderer) < 0){
		throw std::runtime_error("SDL_RenderClear() failed with: " + std::string(SDL_GetError()));
	}
	if(SDL_RenderCopy(renderer, texture, NULL, &draw_area) < 0){
		throw std::runtime_error("SDL_RenderCopy() failed with: " + std::string(SDL_GetError()));
	}
	SDL_RenderPresent(renderer);

	SDL_Delay(5000);

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}


