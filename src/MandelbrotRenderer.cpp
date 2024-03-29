#include <manclbrot/MandelbrotRenderer.h>


MandelbrotRenderer::MandelbrotRenderer(const std::string &name_, unsigned int screen_width_, unsigned int screen_height_, SDL_Texture *texture_):
	MandelbrotRendererInterface(name_, screen_width_, screen_height_, texture_)
{}


void MandelbrotRenderer::draw(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const{
	const auto iter_limit = get_iter_limit();
	for(size_t x=0; x<screen_width; x++){
		for(size_t y=0; y<screen_height; y++){
			const auto test = mandelbrot_test(x*zoom-offset_x, y*zoom-offset_y, iter_limit);
			const auto color = get_color(test);
			write_pixel(pixels, bytes_per_pixel, pitch, x, y, color);
		}
	}
}


double MandelbrotRenderer::square(double &x, double &y) const{
	const double x_sq = x * x;
	const double y_sq = y * y;

	y = 2 * x * y;
	x = x_sq - y_sq;
	return x_sq + y_sq;
}


unsigned long MandelbrotRenderer::mandelbrot_test(double x_0, double y_0, unsigned long limit) const{
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

