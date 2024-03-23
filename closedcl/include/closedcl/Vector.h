#pragma once

#include <closedcl/Buffer.h>

#include <vector>


namespace closedcl{


template<class T>
class Vector : public Buffer{
public:
	Vector(cl_mem buffer_) : Buffer(buffer_){}
	void read(CommandQueue &queue, std::vector<T> &data, size_t num_entries) const{
		data.resize(num_entries);
		Buffer::read(queue, data.data(), data.size()*sizeof(T));
	}
	void read(CommandQueue &queue, std::vector<T> &data) const{
		read(queue, data, data.size());
	}
	void write(CommandQueue &queue, const std::vector<T> &data){
		Buffer::write(queue, data.data(), data.size()*sizeof(T));
	}
};


}
