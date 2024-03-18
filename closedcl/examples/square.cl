__kernel
void square(__global int *input, __global int *output, unsigned long count){
	const size_t idx = get_global_id(0);
	if(idx < count){
		output[idx] = input[idx] * input[idx];
	}
}
