#include <closedcl/CommandQueue.h>
#include <closedcl/Kernel.h>
#include <closedcl/error.h>

#include <stdexcept>


namespace closedcl{


static size_t nearest_multiple(size_t x, size_t y){
	return ((x - 1) / y + 1) * y;
	//return x + (y - x % y) % y;
}


CommandQueue::CommandQueue(cl_command_queue queue_, const device_t &device_):
	queue(queue_),
	device(device_)
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


void CommandQueue::execute(const Kernel &kernel, size_t global, size_t local){
	const std::vector<size_t> global_ = {global};
	const std::vector<size_t> local_ = {local};
	execute(kernel, global_, local_);
}


void CommandQueue::execute(const Kernel &kernel, const std::vector<size_t> &global){
	execute(kernel, global, {});
}


void CommandQueue::execute(const Kernel &kernel, const std::vector<size_t> &global, const std::vector<size_t> &local){
	if(global.empty() || global.size() > device.max_work_item_sizes.size()){
		throw std::logic_error("global work size must have dimension between 1 and " + std::to_string(device.max_work_item_sizes.size()));
	}
	if(!local.empty() && local.size() != global.size()){
		throw std::logic_error("local work size must be of the same dimension as global work size");
	}

	std::vector<size_t> global_ = global;
	const size_t *local_data;
	if(local.empty()){
		for(size_t i=0; i<global_.size(); i++){
			global_[i] = nearest_multiple(global_[i], device.max_work_item_sizes[i]);
		}
		local_data = NULL;
	}else{
		for(size_t i=0; i<global_.size(); i++){
			global_[i] = nearest_multiple(global_[i], local[i]);
		}
		local_data = local.data();
	}

	const auto error = clEnqueueNDRangeKernel(queue, kernel.data(), global_.size(), NULL, global_.data(), local_data, 0, NULL, NULL);
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
