#pragma once

#include <closedcl/Buffer.h>

#include <vector>
#include <stdexcept>


namespace closedcl{


template<class T>
class Vector : public Buffer{
public:
	Vector(cl_mem buffer_, size_t num_entries_) :
		Buffer(buffer_, num_entries_*sizeof(T)),
		m_num_entries(num_entries_)
	{}

	size_t num_entries() const{
		return m_num_entries;
	}

	void read(CommandQueue &queue, std::vector<T> &data) const{
		data.resize(num_entries());
		Buffer::read(queue, data.data());
	}
	void write(CommandQueue &queue, const std::vector<T> &data){
		if(data.size() != num_entries()){
			throw std::logic_error("number of entries does not match");
		}
		Buffer::write(queue, data.data());
	}

private:
	size_t m_num_entries;
};


}
