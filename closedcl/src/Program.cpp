#include <closedcl/Program.h>
#include <closedcl/error.h>

#include <stdexcept>


namespace closedcl{


template<class T>
static size_t buffer_size(const std::vector<T> &vector){
	return vector.size() * sizeof(T);
}



Program::Program(cl_program program_):
	program(program_)
{
}


Program::~Program(){
	clReleaseProgram(program);
}


void Program::build(const std::vector<std::string> &options, const std::vector<device_t> &devices){
	std::string all_options;
	for(const auto &option : options){
		if(!all_options.empty()){
			all_options.push_back(' ');
		}
		all_options += option;
	}
	std::vector<cl_device_id> device_ids;
	for(const auto &device : devices){
		device_ids.push_back(device.id);
	}
	const auto error = clBuildProgram(program, device_ids.size(), device_ids.data(), all_options.c_str(), NULL, NULL);
	if(error != CL_SUCCESS){
		const std::string str_error = "clBuildProgram() failed with: " + error_string(error);
		if(error == CL_BUILD_PROGRAM_FAILURE){
			std::string log;
			for(const auto &device : devices){
				constexpr size_t max_size = 2048;
				char text[max_size];
				size_t actual_size = 0;
				const auto err = clGetProgramBuildInfo(program, device.id, CL_PROGRAM_BUILD_LOG, max_size, text, &actual_size);
				if(err != CL_SUCCESS){
					throw std::runtime_error(str_error + " and clGetProgramBuildInfo(CL_PROGRAM_BUILD_LOG) failed with: " + error_string(err));
				}
				const std::string entry(text, actual_size-1);
				if(!log.empty()){
					log.push_back('\n');
				}
				log += entry;
			}
			throw std::runtime_error(str_error + " and build log:\n" + log);
		}
		throw std::runtime_error(str_error);
	}
}


std::vector<std::pair<device_t, std::string>> Program::get_binaries() const{
	size_t num_devices = 0;
	{
		const auto error = clGetProgramInfo(program, CL_PROGRAM_NUM_DEVICES, sizeof(num_devices), &num_devices, NULL);
		if(error != CL_SUCCESS){
			throw std::runtime_error("clGetProgramInfo(CL_PROGRAM_NUM_DEVICES) failed with: " + error_string(error));
		}
	}
	std::vector<cl_device_id> device_ids(num_devices);
	{
		const auto error = clGetProgramInfo(program, CL_PROGRAM_DEVICES, buffer_size(device_ids), device_ids.data(), NULL);
		if(error != CL_SUCCESS){
			throw std::runtime_error("clGetProgramInfo(CL_PROGRAM_DEVICES) failed with: " + error_string(error));
		}
	}
	std::vector<std::vector<unsigned char>> binaries(num_devices);
	{
		std::vector<size_t> binary_sizes(num_devices);
		const auto error = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, buffer_size(binary_sizes), binary_sizes.data(), NULL);
		if(error != CL_SUCCESS){
			throw std::runtime_error("clGetProgramInfo(CL_PROGRAM_BINARY_SIZES) failed with: " + error_string(error));
		}
		for(size_t i=0; i<num_devices; i++){
			binaries[i].resize(binary_sizes[i]);
		}
	}
	{
		std::vector<unsigned char *> binaries_param;
		for(auto &binary : binaries){
			binaries_param.push_back(binary.data());
		}
		const auto error = clGetProgramInfo(program, CL_PROGRAM_BINARIES, buffer_size(binaries_param), binaries_param.data(), NULL);
		if(error != CL_SUCCESS){
			throw std::runtime_error("clGetProgramInfo(CL_PROGRAM_BINARIES) failed with: " + error_string(error));
		}
	}

	std::vector<std::pair<device_t, std::string>> result;
	for(size_t i=0; i<num_devices; i++){
		std::pair<device_t, std::string> entry;
		entry.first = device_t::from_id(device_ids[i]);
		entry.second = std::string(binaries[i].begin(), binaries[i].end());
		result.push_back(entry);
	}
	return result;
}


std::shared_ptr<Kernel> Program::create_kernel(const std::string &name) const{
	cl_int error = CL_SUCCESS;
	auto kernel = clCreateKernel(program, name.c_str(), &error);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clCreateKernel() failed with: " + error_string(error));
	}
	return std::make_shared<Kernel>(kernel);
}


}
