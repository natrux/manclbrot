// Squares the complex number c = (x + y*i) and returns |c|^2.
static double square(double &x, double &y){
	const double x_sq = x * x;
	const double y_sq = y * y;

	y = 2 * x * y;
	x = x_sq - y_sq;
	return x_sq + y_sq;
}


// Performs limit many iterations to test if c = (x_0 + y_0 * i) is likely in the Mandelbrot Set.
// Returns 0 if yes, otherwise the smallest n such that |z_n| > 2 (providing a witness for the opposite).
static unsigned long mandelbrot_test(double x_0, double y_0, unsigned long limit){
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



int main(int /*argc*/, char **/*argv*/){
	return 0;
}


