#pragma once

#include <closedcl/platform_t.h>
#include <closedcl/device_t.h>
#include <closedcl/CommandQueue.h>
#include <closedcl/Program.h>

#include <memory>



namespace closedcl{


class Context{
public:
	Context();
	Context(const platform_t &platform);
	Context(const platform_t &platform, cl_device_type type);
	Context(const platform_t &platform, const std::vector<device_t> &devices);
	~Context();
	Context(const Context &other) = delete;
	Context &operator=(const Context &other) = delete;

	std::vector<device_t> get_devices() const;
	std::shared_ptr<CommandQueue> create_queue();
	std::shared_ptr<CommandQueue> create_queue(const device_t &device);
	std::shared_ptr<Program> create_program(const std::vector<std::string> &sources);
	std::shared_ptr<Buffer> create_buffer(cl_mem_flags flags, size_t size);
	template<class T>
	std::shared_ptr<Vector<T>> create_vector(cl_mem_flags flags, size_t num_entries){
		auto buffer = create_buffer_internal(flags, num_entries*sizeof(T));
		return std::make_shared<Vector<T>>(buffer);
	}
	template<class T, size_t N>
	std::shared_ptr<Array<T, N>> create_array(cl_mem_flags flags){
		auto buffer = create_buffer_internal(flags, N*sizeof(T));
		return std::make_shared<Array<T, N>>(buffer);
	}

private:
	cl_context context;
	std::vector<device_t> devices;
	cl_mem create_buffer_internal(cl_mem_flags flags, size_t size);
};


}
