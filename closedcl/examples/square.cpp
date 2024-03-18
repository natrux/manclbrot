#include <closedcl/Context.h>

#include <fstream>
#include <iostream>


static void square(){
	closedcl::Context context;
	auto queue = context.create_queue();

	const std::string path = "closedcl/examples/square.cl";
	std::ifstream stream(path);
	if(!stream){
		throw std::runtime_error("Opening file " + path + " failed");
	}
	const std::string src = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
	auto program = context.create_program({src});
	program->build({"-cl-kernel-arg-info"}, context.get_devices());

	constexpr size_t count = 10;
	const std::array<int, count> input = {-2, -1, 0, 1, 2, 3, 4, 5, 6, 7};
	std::array<int, count> output;
	auto input_cl = context.create_array<int, count>(CL_MEM_READ_ONLY);
	auto output_cl = context.create_array<int, count>(CL_MEM_WRITE_ONLY);

	auto kernel = program->create_kernel("square");
	kernel->set_argument("input", *input_cl);
	kernel->set_argument("output", *output_cl);
	kernel->set_argument("count", count);

	input_cl->write(*queue, input);
	queue->execute(*kernel, count);
	queue->finish();
	output_cl->read(*queue, output);

	for(size_t i=0; i<count; i++){
		std::cout << "( " << input[i] << " ) ^ 2 = " << output[i] << std::endl;
	}
}


int main(int /*argc*/, char **/*argv*/){
	try{
		square();
	}catch(const std::exception &err){
		std::cerr << err.what() << std::endl;
		return 1;
	}

	return 0;
}


