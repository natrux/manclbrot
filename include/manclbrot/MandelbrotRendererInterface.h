#pragma once

#include <vector>
#include <random>

#include <SDL2/SDL.h>


class MandelbrotRendererInterface{
public:
	MandelbrotRendererInterface(int screen_width, int screen_height, SDL_Texture *texture);
	virtual ~MandelbrotRendererInterface();
	void set_iter_limit(unsigned long iter_limit);
	void renew_colors();
	virtual void draw(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const = 0;
	void draw(double zoom, double offset_x, double offset_y) const;

protected:
	int screen_width;
	int screen_height;
	uint32_t get_color(size_t iter) const;
	unsigned long get_iter_limit() const;

private:
	struct color_gen_t{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		int8_t dr = 0;
		int8_t dg = 0;
		int8_t db = 0;
	};
	std::random_device random_device;
	std::mt19937_64 random_generator;
	std::vector<uint32_t> colors;
	std::vector<color_gen_t> color_gen;
	SDL_Texture *texture = NULL;
	SDL_PixelFormat *pixel_format;
};
