#include <manclbrot/MandelbrotRenderer.h>
#include <manclbrot/CL_MandelbrotRenderer.h>
#include <closedcl/Context.h>

#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>

#include <SDL2/SDL.h>


static void read_args(const std::vector<std::string> &args, bool &usage, int &screen_width, int &screen_height, bool &use_opencl){
	auto iter = args.begin();
	while(iter != args.end()){
		const std::string &arg = *iter;
		if(arg == "--help"){
			usage = true;
		}else if(arg == "--screen_width"){
			iter++;
			if(iter != args.end()){
				screen_width = std::stoi(*iter);
			}
		}else if(arg == "--screen_height"){
			iter++;
			if(iter != args.end()){
				screen_height = std::stoi(*iter);
			}
		}else if(arg == "--use_opencl"){
			iter++;
			if(iter != args.end()){
				const std::string &value = *iter;
				if(value == "true"){
					use_opencl = true;
				}else if(value == "false"){
					use_opencl = false;
				}
			}
		}
		iter++;
	}
}


static void print_usage(const std::string &name){
	std::cout << "Usage: " << name << " [OPTION VALUE]" << std::endl;
	std::cout << "Where OPTIONs are any of the following:" << std::endl;
	std::cout << "\t--help: Show this usage and exit" << std::endl;
	std::cout << "\t--screen_width <INT>: Set the screen width" << std::endl;
	std::cout << "\t--screen_height <INT>: Set the screen height" << std::endl;
	std::cout << "\t--use_opencl <BOOL>: Whether to use OpenCL rendering" << std::endl;
}


static void manclbrot(int screen_width, int screen_height, bool use_opencl){
	double zoom = 0.005;
	double offset_x = screen_width * zoom * 2 / 3;
	double offset_y = screen_height * zoom / 2;
	unsigned long iter_limit = 100;

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

	std::vector<std::unique_ptr<MandelbrotRendererInterface>> mandelbrot_renderers;
	if(use_opencl){
		try{
			auto mandelbrot_renderer = std::make_unique<CL_MandelbrotRenderer>("OpenCL", screen_width, screen_height, texture, true);
			mandelbrot_renderer->set_iter_limit(iter_limit);
			mandelbrot_renderers.push_back(std::move(mandelbrot_renderer));
		}catch(const std::exception &err){
			std::cerr << "Failed to create OpenCL Mandelbrot renderer: " << err.what() << std::endl;
		}
	}
	try{
		auto mandelbrot_renderer = std::make_unique<MandelbrotRenderer>("CPU", screen_width, screen_height, texture);
		mandelbrot_renderer->set_iter_limit(iter_limit);
		mandelbrot_renderers.push_back(std::move(mandelbrot_renderer));
	}catch(const std::exception &err){
		std::cerr << "Failed to create Mandelbrot renderer: " << err.what() << std::endl;
	}
	if(mandelbrot_renderers.empty()){
		throw std::runtime_error("No renderers available");
	}
	auto current_renderer = mandelbrot_renderers.begin();

	bool do_run = true;
	bool do_draw = true;
	while(do_run){
		auto &mandelbrot_renderer = *current_renderer;
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
			case SDLK_r:
				current_renderer++;
				if(current_renderer == mandelbrot_renderers.end()){
					current_renderer = mandelbrot_renderers.begin();
				}
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
}


int main(int argc, char **argv){
	int screen_width = 1280;
	int screen_height = 720;
	bool use_opencl = true;
	try{
		bool usage = false;
		const std::string name = argv[0];
		std::vector<std::string> args;
		for(int i=1; i<argc; i++){
			args.push_back(argv[i]);
		}
		read_args(args, usage, screen_width, screen_height, use_opencl);
		if(usage){
			print_usage(name);
			return 0;
		}
	}catch(const std::exception &err){
		std::cerr << err.what() << std::endl;
		return 1;
	}

	try{
		manclbrot(screen_width, screen_height, use_opencl);
	}catch(const std::exception &err){
		std::cerr << err.what() << std::endl;
		return 1;
	}

	return 0;
}





