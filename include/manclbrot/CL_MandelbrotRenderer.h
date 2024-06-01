#pragma once

#include <manclbrot/MandelbrotRendererInterface.h>

#include <wideopencl/Context.h>



class CL_MandelbrotRenderer : public MandelbrotRendererInterface{
public:
	CL_MandelbrotRenderer(const std::string &name, unsigned int screen_width, unsigned int screen_height, SDL_Texture *texture, bool with_buffer);
	void draw(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const override;

	void draw_with_pixels(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const;
	void draw_with_buffer(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const;

private:
	bool with_buffer = true;
	wideopencl::Context context;
	std::shared_ptr<wideopencl::CommandQueue> queue;
	std::shared_ptr<wideopencl::Program> mandelbrot;
	std::shared_ptr<wideopencl::Kernel> mandelbrot_rect_kernel;
	std::shared_ptr<wideopencl::Kernel> mandelbrot_rect_plot_kernel;
};
