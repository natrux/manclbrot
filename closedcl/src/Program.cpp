#include <closedcl/Program.h>
#include <closedcl/error.h>

#include <stdexcept>


namespace closedcl{


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
		}else{
			throw std::runtime_error(str_error);
		}
	}
}


std::shared_ptr<Kernel> Program::create_kernel(const std::string &name){
	cl_int error = CL_SUCCESS;
	auto kernel = clCreateKernel(program, name.c_str(), &error);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clCreateKernel() failed with: " + error_string(error));
	}
	return std::make_shared<Kernel>(kernel);
}


}
