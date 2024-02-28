#include <closedcl/Kernel.h>
#include <closedcl/CommandQueue.h>
#include <closedcl/error.h>

#include <vector>
#include <stdexcept>


namespace closedcl{


Kernel::Kernel(cl_kernel kernel_, size_t max_name_size):
	kernel(kernel_)
{
	cl_uint num_arguments = 0;
	{
		const auto error = clGetKernelInfo(kernel, CL_KERNEL_NUM_ARGS, sizeof(num_arguments), &num_arguments, NULL);
		if(error != CL_SUCCESS){
			throw std::runtime_error("clGetKernelInfo(CL_KERNEL_NUM_ARGS) failed with: " + error_string(error));
		}
	}
	for(cl_uint i=0; i<num_arguments; i++){
		std::vector<char> name_chr(max_name_size);
		size_t actual_size = 0;
		const auto error = clGetKernelArgInfo(kernel, i, CL_KERNEL_ARG_NAME, name_chr.size(), name_chr.data(), &actual_size);
		if(error != CL_SUCCESS){
			throw std::runtime_error("clGetKernelArgInfo(CL_KERNEL_ARG_NAME) failed with: " + error_string(error));
		}
		name_chr.resize(actual_size);

		const std::string name(name_chr.begin(), name_chr.end());
		arguments[name] = i;
	}
}


Kernel::~Kernel(){
	clReleaseKernel(kernel);
}


void Kernel::execute(CommandQueue &queue, size_t global){
	queue.execute(*this, global);
}


cl_kernel Kernel::data() const{
	return kernel;
}


cl_uint Kernel::find_argument(const std::string &name){
	const auto find = arguments.find(name);
	if(find == arguments.end()){
		throw std::logic_error("No such argument: " + name);
	}
	return find->second;
}


void Kernel::set_argument_internal(cl_uint index, size_t size, const void *value){
	const auto error = clSetKernelArg(kernel, index, size, value);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clSetKernelArg() failed with: " + error_string(error));
	}
}


}
