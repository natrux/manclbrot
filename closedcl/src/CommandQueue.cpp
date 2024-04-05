#include <closedcl/CommandQueue.h>
#include <closedcl/Kernel.h>
#include <closedcl/error.h>

#include <stdexcept>


namespace closedcl{


CommandQueue::CommandQueue(cl_command_queue queue_):
	queue(queue_)
{
}


CommandQueue::~CommandQueue(){
	clReleaseCommandQueue(queue);
}


void CommandQueue::read(const Buffer &buffer, void *data){
	const cl_bool blocking = CL_TRUE;
	const auto error = clEnqueueReadBuffer(queue, buffer.data(), blocking, 0, buffer.num_bytes(), data, 0, NULL, NULL);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clEnqueueReadBuffer() failed with: " + error_string(error));
	}
}


void CommandQueue::write(Buffer &buffer, const void *data){
	// if blocking, data is also copied and the memory can be reused.
	const cl_bool blocking = CL_TRUE;
	const auto error = clEnqueueWriteBuffer(queue, buffer.data(), blocking, 0, buffer.num_bytes(), data, 0, NULL, NULL);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clEnqueueWriteBuffer() failed with: " + error_string(error));
	}
}


void CommandQueue::execute(const Kernel &kernel, size_t global){
	const std::vector<size_t> global_ = {global};
	execute(kernel, global_);
}


void CommandQueue::execute(const Kernel &kernel, const std::vector<size_t> &global){
	if(global.empty()){
		throw std::logic_error("global work size must have at least one dimension");
	}
	const auto error = clEnqueueNDRangeKernel(queue, kernel.data(), global.size(), NULL, global.data(), NULL, 0, NULL, NULL);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clEnqueueNDRangeKernel() failed with: " + error_string(error));
	}
}


void CommandQueue::flush(){
	const auto error = clFlush(queue);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clFlush() failed with: " + error_string(error));
	}
}


void CommandQueue::finish(){
	const auto error = clFinish(queue);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clFinish() failed with: " + error_string(error));
	}
}


}
