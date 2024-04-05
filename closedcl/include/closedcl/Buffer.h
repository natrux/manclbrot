#pragma once

#include <CL/cl.h>


namespace closedcl{


class CommandQueue;

class Buffer{
public:
	Buffer(cl_mem buffer, size_t num_bytes);
	~Buffer();
	Buffer(const Buffer &other) = delete;
	Buffer &operator=(const Buffer &other) = delete;

	void read(CommandQueue &queue, void *data) const;
	void write(CommandQueue &queue, const void *data);

	size_t num_bytes() const;
	cl_mem data() const;

private:
	cl_mem buffer;
	size_t m_num_bytes;
};


}
