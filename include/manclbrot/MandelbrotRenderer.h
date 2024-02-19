#include <manclbrot/MandelbrotRendererInterface.h>


class MandelbrotRenderer : public MandelbrotRendererInterface{
public:
	MandelbrotRenderer(int screen_width, int screen_height, SDL_Texture *texture);
	void draw(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const override;

private:
	// Squares the complex number c = (x + y*i) and returns |c|^2.
	double square(double &x, double &y) const;

	// Performs limit many iterations to test if c = (x_0 + y_0 * i) is likely in the Mandelbrot Set.
	// Returns 0 if yes, otherwise the smallest n such that |z_n| > 2 (providing a witness for the opposite).
	unsigned long mandelbrot_test(double x_0, double y_0, unsigned long limit) const;

	void write_pixel(void *pixels, uint8_t bytes_per_pixel, int pitch, int x, int y, uint32_t color) const;
};

