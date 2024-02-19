#include <manclbrot/MandelbrotRendererInterface.h>

#include <chrono>
#include <stdexcept>


MandelbrotRendererInterface::MandelbrotRendererInterface(int screen_width_, int screen_height_, SDL_Texture *texture_):
	screen_width(screen_width_),
	screen_height(screen_height_),
	random_generator(random_device()),
	texture(texture_)
{
	const auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	const auto seed = static_cast<decltype(random_generator)::result_type>(time);
	random_generator.seed(seed);

	uint32_t raw_pixel_format;
	if(SDL_QueryTexture(texture_, &raw_pixel_format, NULL, NULL, NULL) < 0){
		throw std::runtime_error("SDL_QueryTexture() failed with: " + std::string(SDL_GetError()));
	}
	pixel_format = SDL_AllocFormat(raw_pixel_format);
	if(!pixel_format){
		throw std::runtime_error("SDL_AllocFormat() failed with: " + std::string(SDL_GetError()));
	}

	color_gen_t color;
	colors.push_back(SDL_MapRGBA(pixel_format, color.r, color.g, color.b, 255));
	color_gen.push_back(color);
}


MandelbrotRendererInterface::~MandelbrotRendererInterface(){
	SDL_FreeFormat(pixel_format);
}


void MandelbrotRendererInterface::set_iter_limit(unsigned long iter_limit_){
	if(iter_limit_ <= 0){
		throw std::logic_error("iteration limit must be positive");
	}

	const uint8_t min_delta = 1;
	const uint8_t max_delta = 20;
	std::uniform_int_distribution<int8_t> distribution(min_delta, max_delta);

	colors.resize(iter_limit_);
	color_gen.resize(iter_limit_);
	const auto &last_color = color_gen.back();
	int16_t r = last_color.r;
	int16_t g = last_color.g;
	int16_t b = last_color.b;
	int8_t dr = last_color.dr;
	int8_t dg = last_color.dg;
	int8_t db = last_color.db;
	for(unsigned long i=iter_limit; i<iter_limit_; i++){
		if(r == 0 || r == 255){
			const int8_t d = distribution(random_generator);
			dr = (r == 0) ? d : -d;
		}
		if(g == 0 || g == 255){
			const int8_t d = distribution(random_generator);
			dg = (g == 0) ? d : -d;
		}
		if(b == 0 || b == 255){
			const int8_t d = distribution(random_generator);
			db = (b == 0) ? d : -d;
		}

		r = std::min(std::max(r+dr, 0), 255);
		g = std::min(std::max(g+dg, 0), 255);
		b = std::min(std::max(b+db, 0), 255);

		color_gen_t color;
		color.r = r;
		color.g = g;
		color.b = b;
		color.dr = dr;
		color.dg = dg;
		color.db = db;
		color_gen[i] = color;
		colors[i] = SDL_MapRGBA(pixel_format, r, g, b, 255);
	}
	iter_limit = iter_limit_;
}


void MandelbrotRendererInterface::renew_colors(){
	const auto old_limit = iter_limit;
	set_iter_limit(1);
	set_iter_limit(old_limit);
}


void MandelbrotRendererInterface::draw(double zoom, double offset_x, double offset_y) const{
	void *pixels;
	int pitch;
	if(SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0){
		throw std::runtime_error("SDL_LockTexture() failed with: " + std::string(SDL_GetError()));
	}
	draw(zoom, offset_x, offset_y, pixels, pixel_format->BytesPerPixel, pitch);
	SDL_UnlockTexture(texture);
}


uint32_t MandelbrotRendererInterface::get_color(size_t iter) const{
	return colors.at(iter);
}

