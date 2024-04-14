#include <manclbrot/CL_MandelbrotRenderer.h>

#include <fstream>
#include <stdexcept>



CL_MandelbrotRenderer::CL_MandelbrotRenderer(const std::string &name_, unsigned int screen_width_, unsigned int screen_height_, SDL_Texture *texture_, bool with_buffer_):
	MandelbrotRendererInterface(name_, screen_width_, screen_height_, texture_),
	with_buffer(with_buffer_)
{
	queue = context.create_queue();

	const std::string path = "kernel/mandelbrot.cl";
	std::ifstream stream(path);
	if(!stream){
		throw std::runtime_error("Opening file " + path + " failed");
	}
	const std::string src = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
	{
		auto other_program = context.create_program({src});
		other_program->build({"-cl-kernel-arg-info"}, context.get_devices());
		const auto binaries = other_program->get_binaries();
		mandelbrot = context.create_program(binaries);
	}
	mandelbrot->build({"-cl-kernel-arg-info"}, context.get_devices());
	mandelbrot_rect_kernel = mandelbrot->create_kernel("mandelbrot_rect");
	mandelbrot_rect_plot_kernel = mandelbrot->create_kernel("mandelbrot_rect_plot");
}


void CL_MandelbrotRenderer::draw(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const{
	if(with_buffer){
		draw_with_buffer(zoom, offset_x, offset_y, pixels, bytes_per_pixel, pitch);
	}else{
		draw_with_pixels(zoom, offset_x, offset_y, pixels, bytes_per_pixel, pitch);
	}
}


void CL_MandelbrotRenderer::draw_with_pixels(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const{
	const size_t num_pixels = screen_width * screen_height;
	std::vector<unsigned long> result;
	auto result_cl = context.create_vector<unsigned long>(CL_MEM_WRITE_ONLY, num_pixels);

	mandelbrot_rect_kernel->set_argument("zoom", zoom);
	mandelbrot_rect_kernel->set_argument("offset_x", offset_x);
	mandelbrot_rect_kernel->set_argument("offset_y", offset_y);
	mandelbrot_rect_kernel->set_argument("limit", get_iter_limit());
	mandelbrot_rect_kernel->set_argument("result", *result_cl);
	mandelbrot_rect_kernel->set_argument("count_x", screen_width);
	mandelbrot_rect_kernel->set_argument("count_y", screen_height);

	queue->execute(*mandelbrot_rect_kernel, {screen_width, screen_height});
	queue->finish();
	result_cl->read(*queue, result);

	for(size_t i=0; i<screen_width; i++){
		for(size_t j=0; j<screen_height; j++){
			const auto test = result[i*screen_height + j];
			const auto color = get_color(test);
			write_pixel(pixels, bytes_per_pixel, pitch, i, j, color);
		}
	}
}


void CL_MandelbrotRenderer::draw_with_buffer(double zoom, double offset_x, double offset_y, void *pixels, uint8_t bytes_per_pixel, int pitch) const{
	const size_t data_size = screen_height * static_cast<unsigned int>(pitch);
	const auto iter_limit = get_iter_limit();
	auto pixels_cl = context.create_buffer(CL_MEM_WRITE_ONLY, data_size);
	pixels_cl->write(*queue, pixels);
	auto colors_cl = context.create_vector<uint32_t>(CL_MEM_READ_ONLY, iter_limit);
	colors_cl->write(*queue, get_colors());

	mandelbrot_rect_plot_kernel->set_argument("zoom", zoom);
	mandelbrot_rect_plot_kernel->set_argument("offset_x", offset_x);
	mandelbrot_rect_plot_kernel->set_argument("offset_y", offset_y);
	mandelbrot_rect_plot_kernel->set_argument("limit", iter_limit);
	mandelbrot_rect_plot_kernel->set_argument("count_x", screen_width);
	mandelbrot_rect_plot_kernel->set_argument("count_y", screen_height);
	mandelbrot_rect_plot_kernel->set_argument("colors", *colors_cl);
	mandelbrot_rect_plot_kernel->set_argument("pixels", *pixels_cl);
	mandelbrot_rect_plot_kernel->set_argument("bytes_per_pixel", bytes_per_pixel);
	mandelbrot_rect_plot_kernel->set_argument("pitch", pitch);

	queue->execute(*mandelbrot_rect_plot_kernel, {screen_width, screen_height});
	queue->finish();
	pixels_cl->read(*queue, pixels);
}

