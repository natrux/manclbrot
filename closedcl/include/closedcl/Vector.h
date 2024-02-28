#pragma once

#include <closedcl/Buffer.h>

#include <vector>


namespace closedcl{


template<class T>
class Vector : public Buffer{
public:
	Vector(cl_mem buffer_) : Buffer(buffer_){}
	void read(CommandQueue &queue, std::vector<T> &data, size_t num_entries);
	void write(CommandQueue &queue, const std::vector<T> &data);
};


}
