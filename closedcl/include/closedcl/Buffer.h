#pragma once

#include <CL/cl.h>


namespace closedcl{


class CommandQueue;

class Buffer{
public:
	Buffer(cl_mem buffer);
	~Buffer();
	Buffer(const Buffer &other) = delete;
	Buffer &operator=(const Buffer &other) = delete;

	void read(CommandQueue &queue, void *data, size_t size);
	void write(CommandQueue &queue, const void *data, size_t size);

	cl_mem data() const;

private:
	cl_mem buffer;
};


}
