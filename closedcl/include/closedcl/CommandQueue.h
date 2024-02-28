#pragma once

#include <closedcl/Buffer.h>
#include <closedcl/Vector.h>

#include <vector>

#include <CL/cl.h>


namespace closedcl{


class Kernel;

class CommandQueue{
public:
	CommandQueue(cl_command_queue queue);
	~CommandQueue();
	CommandQueue(const CommandQueue &other) = delete;
	CommandQueue &operator=(const CommandQueue &other) = delete;

	void read(const Buffer &buffer, void *data, size_t size);
	void write(Buffer &buffer, const void *data, size_t size);
	template<class T>
	void read(const Vector<T> &buffer, std::vector<T> &data, size_t num_entries){
		data.resize(num_entries);
		read(buffer, data.data(), data.size()*sizeof(T));
	}
	template<class T>
	void write(Vector<T> &buffer, const std::vector<T> &data){
		write(buffer, data.data(), data.size()*sizeof(T));
	}
	void execute(const Kernel &kernel, size_t global);
	void execute(const Kernel &kernel, const std::vector<size_t> &global);
	void flush();
	void finish();

private:
	cl_command_queue queue;
};


}
