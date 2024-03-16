#pragma once

#include <closedcl/Buffer.h>

#include <array>


namespace closedcl{


template<class T, size_t N>
class Array : public Buffer{
public:
	Array(cl_mem buffer_) : Buffer(buffer_){}
	void read(CommandQueue &queue, std::array<T, N> &data);
	void write(CommandQueue &queue, const std::array<T, N> &data);
};


}
