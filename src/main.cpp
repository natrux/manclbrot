#include <manclbrot/MandelbrotRenderer.h>
#include <manclbrot/CL_MandelbrotRenderer.h>
#include <closedcl/Context.h>

#include <string>
#include <stdexcept>
#include <memory>

#include <SDL2/SDL.h>



int main(int /*argc*/, char **/*argv*/){
	const unsigned int screen_width = 1280;
	const unsigned int screen_height = 720;
	double zoom = 0.005;
	double offset_x = screen_width * zoom * 2 / 3;
	double offset_y = screen_height * zoom / 2;
	unsigned long iter_limit = 100;
	const bool use_opencl = true;

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
		mandelbrot_renderer = std::make_unique<CL_MandelbrotRenderer>(screen_width, screen_height, texture, true);
	}else{
		mandelbrot_renderer = std::make_unique<MandelbrotRenderer>(screen_width, screen_height, texture);
	}
	mandelbrot_renderer->set_iter_limit(iter_limit);

	bool do_run = true;
	bool do_draw = true;
	while(do_run){
		if(do_draw){
			mandelbrot_renderer->draw(zoom, offset_x, offset_y);
			if(SDL_RenderClear(renderer) < 0){
				throw std::runtime_error("SDL_RenderClear() failed with: " + std::string(SDL_GetError()));
			}
			if(SDL_RenderCopy(renderer, texture, NULL, &draw_area) < 0){
				throw std::runtime_error("SDL_RenderCopy() failed with: " + std::string(SDL_GetError()));
			}
			SDL_RenderPresent(renderer);
			do_draw = false;
		}

		SDL_Event event;
		if(SDL_WaitEvent(&event) == 0){
			do_run = false;
		}

		if(event.type == SDL_QUIT){
			do_run = false;
		}else if(event.type == SDL_KEYDOWN && event.key.type == SDL_KEYDOWN){
			switch(event.key.keysym.sym){
			case SDLK_ESCAPE:
				do_run = false;
				break;
			case SDLK_LEFT:
				offset_x += 50*zoom;
				do_draw = true;
				break;
			case SDLK_RIGHT:
				offset_x -= 50*zoom;
				do_draw = true;
				break;
			case SDLK_UP:
				offset_y += 50*zoom;
				do_draw = true;
				break;
			case SDLK_DOWN:
				offset_y -= 50*zoom;
				do_draw = true;
				break;
			case SDLK_PLUS:
				zoom /= 2;
				offset_x -= (screen_width / 2) * zoom;
				offset_y -= (screen_height / 2) * zoom;
				do_draw = true;
				break;
			case SDLK_MINUS:
				offset_x += (screen_width / 2) * zoom;
				offset_y += (screen_height / 2) * zoom;
				do_draw = true;
				zoom *= 2;
				break;
			case SDLK_SPACE:
				mandelbrot_renderer->renew_colors();
				do_draw = true;
				break;
			case SDLK_PAGEUP:
				iter_limit += 10;
				mandelbrot_renderer->set_iter_limit(iter_limit);
				do_draw = true;
				break;
			case SDLK_PAGEDOWN:
				iter_limit -= std::min(static_cast<unsigned long>(10), iter_limit-1);
				mandelbrot_renderer->set_iter_limit(iter_limit);
				do_draw = true;
				break;
			default:
				break;
			}
		}
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}


