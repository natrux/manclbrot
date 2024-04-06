#include <closedcl/Context.h>
#include <closedcl/error.h>

#include <stdexcept>


namespace closedcl{


Context::Context():
	Context(platform_t::choose())
{
}


Context::Context(const platform_t &platform_):
	Context(platform_, device_t::find(platform_))
{
}


Context::Context(const platform_t &platform_, cl_device_type type):
	Context(platform_, device_t::find(platform_, type))
{
}


Context::Context(const platform_t &platform_, const std::vector<device_t> &devices_):
	platform(platform_),
	devices(devices_)
{
	const cl_context_properties properties[] = {
		CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform.id),
		0
	};
	std::vector<cl_device_id> device_ids;
	for(const auto &device : devices){
		device_ids.push_back(device.id);
	}
	cl_int error = CL_SUCCESS;
	context = clCreateContext(properties, device_ids.size(), device_ids.data(), NULL, NULL, &error);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clCreateContext() failed with: " + error_string(error));
	}
}


Context::Context(const platform_t &platform_, const std::string &device_name):
	Context(platform_, {device_t::find(platform_, device_name)})
{
}


Context::Context(const std::string &platform_name):
	Context(platform_t::find(platform_name))
{
}


Context::Context(const std::string &platform_name, cl_device_type type):
	Context(platform_t::find(platform_name), type)
{
}


Context::Context(const std::string &platform_name, const std::string &device_name):
	Context(platform_t::find(platform_name), device_name)
{
}


Context::~Context(){
	clReleaseContext(context);
}


platform_t Context::get_platform() const{
	return platform;
}


std::vector<device_t> Context::get_devices() const{
	return devices;
}


std::shared_ptr<CommandQueue> Context::create_queue() const{
	if(devices.empty()){
		throw std::logic_error("No devices in current OpenCL context");
	}
	return create_queue(devices.front());
}


std::shared_ptr<CommandQueue> Context::create_queue(const device_t &device) const{
	const cl_command_queue_properties properties = 0;
	cl_int error = CL_SUCCESS;
	auto queue = clCreateCommandQueue(context, device.id, properties, &error);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clCreateCommandQueue() failed with: " + error_string(error));
	}
	return std::make_shared<CommandQueue>(queue, device);
}


std::shared_ptr<Program> Context::create_program(const std::vector<std::string> &sources) const{
	std::vector<const char *> sources_;
	std::vector<size_t> sizes;
	for(const auto &source : sources){
		sources_.push_back(source.c_str());
		sizes.push_back(source.length());
	}
	cl_int error = CL_SUCCESS;
	auto program = clCreateProgramWithSource(context, sources_.size(), sources_.data(), sizes.data(), &error);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clCreateProgram() failed with: " + error_string(error));
	}
	return std::make_shared<Program>(program);
}


std::shared_ptr<Buffer> Context::create_buffer(cl_mem_flags flags, size_t size) const{
	auto buffer = create_buffer_internal(flags, size);
	return std::make_shared<Buffer>(buffer, size);
}


cl_mem Context::create_buffer_internal(cl_mem_flags flags, size_t size) const{
	cl_int error = CL_SUCCESS;
	cl_mem buffer = clCreateBuffer(context, flags, size, NULL, &error);
	if(error != CL_SUCCESS){
		throw std::runtime_error("clCreateBuffer() failed with: " + error_string(error));
	}
	return buffer;
}


}
