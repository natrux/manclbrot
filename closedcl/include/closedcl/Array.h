#pragma once

#include <closedcl/Buffer.h>

#include <array>


namespace closedcl{


template<class T, size_t N>
class Array : public Buffer{
public:
	Array(cl_mem buffer_) :
		Buffer(buffer_, N*sizeof(T))
	{}

	size_t num_entries() const{
		return N;
	}

	void read(CommandQueue &queue, std::array<T, N> &data) const{
		Buffer::read(queue, data.data());
	}
	void write(CommandQueue &queue, const std::array<T, N> &data){
		Buffer::write(queue, data.data());
	}
};


}
