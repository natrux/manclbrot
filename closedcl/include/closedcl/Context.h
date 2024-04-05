#pragma once

#include <closedcl/platform_t.h>
#include <closedcl/device_t.h>
#include <closedcl/CommandQueue.h>
#include <closedcl/Program.h>
#include <closedcl/Vector.h>
#include <closedcl/Array.h>

#include <memory>



namespace closedcl{


class Context{
public:
	Context();
	Context(const platform_t &platform);
	Context(const platform_t &platform, cl_device_type type);
	Context(const platform_t &platform, const std::vector<device_t> &devices);
	Context(const platform_t &platform, const std::string &device_name);
	Context(const std::string &platform_name);
	Context(const std::string &platform_name, cl_device_type type);
	Context(const std::string &platform_name, const std::string &device_name);
	~Context();
	Context(const Context &other) = delete;
	Context &operator=(const Context &other) = delete;

	platform_t get_platform() const;
	std::vector<device_t> get_devices() const;
	std::shared_ptr<CommandQueue> create_queue() const;
	std::shared_ptr<CommandQueue> create_queue(const device_t &device) const;
	std::shared_ptr<Program> create_program(const std::vector<std::string> &sources) const;
	std::shared_ptr<Buffer> create_buffer(cl_mem_flags flags, size_t size) const;
	template<class T>
	std::shared_ptr<Vector<T>> create_vector(cl_mem_flags flags, size_t num_entries) const{
		auto buffer = create_buffer_internal(flags, num_entries*sizeof(T));
		return std::make_shared<Vector<T>>(buffer, num_entries);
	}
	template<class T, size_t N>
	std::shared_ptr<Array<T, N>> create_array(cl_mem_flags flags) const{
		auto buffer = create_buffer_internal(flags, N*sizeof(T));
		return std::make_shared<Array<T, N>>(buffer);
	}

private:
	cl_context context;
	platform_t platform;
	std::vector<device_t> devices;
	cl_mem create_buffer_internal(cl_mem_flags flags, size_t size) const;
};


}
