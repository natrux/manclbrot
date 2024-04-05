#pragma once

#include <closedcl/Buffer.h>

#include <vector>
#include <array>

#include <CL/cl.h>


namespace closedcl{


class Kernel;

class CommandQueue{
public:
	CommandQueue(cl_command_queue queue);
	~CommandQueue();
	CommandQueue(const CommandQueue &other) = delete;
	CommandQueue &operator=(const CommandQueue &other) = delete;

	void read(const Buffer &buffer, void *data);
	void write(Buffer &buffer, const void *data);
	void execute(const Kernel &kernel, size_t global);
	void execute(const Kernel &kernel, const std::vector<size_t> &global);
	void flush();
	void finish();

private:
	cl_command_queue queue;
};


}
