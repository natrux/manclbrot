double square(double *x, double *y){
	const double x_sq = *x * *x;
	const double y_sq = *y * *y;
	*y = 2 * *x * *y;
	*x = x_sq - y_sq;
	return x_sq + y_sq;
}


unsigned long mandelbrot_iter(double x_0, double y_0, unsigned long limit){
	double x = x_0;
	double y = y_0;
	for(unsigned int i=1; i<limit; i++){
		const double abs_sq = square(&x, &y);
		if(abs_sq > 4){
			return i;
		}
		x += x_0;
		y += y_0;
	}
	return 0;
}


__kernel
void mandelbrot_list(__global double *x_0, __global double *y_0, __global unsigned long *result, unsigned long limit, unsigned long count){
	const size_t idx = get_global_id(0);
	if(idx < count){
		result[idx] = mandelbrot_iter(x_0[idx], y_0[idx], limit);
	}
}


__kernel
void mandelbrot_rect(double zoom, double offset_x, double offset_y, unsigned long limit, __global unsigned long *result, unsigned int count_x, unsigned int count_y){
	const size_t idx = get_global_id(0);
	const size_t idy = get_global_id(1);
	if(idx < count_x && idy < count_y){
		const double x_0 = idx * zoom - offset_x;
		const double y_0 = idy * zoom - offset_y;
		result[idx * count_y + idy] = mandelbrot_iter(x_0, y_0, limit);
	}
}


__kernel
void mandelbrot_rect_plot(double zoom, double offset_x, double offset_y, unsigned long limit, unsigned int count_x, unsigned int count_y, __global unsigned int *colors, __global unsigned char *pixels, unsigned char bytes_per_pixel, int pitch){
	const size_t idx = get_global_id(0);
	const size_t idy = get_global_id(1);
	if(idx < count_x && idy < count_y){
		const double x_0 = idx * zoom - offset_x;
		const double y_0 = idy * zoom - offset_y;
		const unsigned long iter = mandelbrot_iter(x_0, y_0, limit);
		const unsigned int color = colors[iter];
		const unsigned int pixel_offset = idy * pitch + idx * bytes_per_pixel;
		for(unsigned char p=0; p<bytes_per_pixel; p++){
			pixels[pixel_offset+p] = ((unsigned char *)&color)[4-bytes_per_pixel+p];
		}
	}
}


