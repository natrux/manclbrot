#pragma once

#include <closedcl/Vector.h>
#include <closedcl/CommandQueue.h>


namespace closedcl{


template<class T>
void Vector<T>::read(CommandQueue &queue, std::vector<T> &data, size_t num_entries){
	queue.read(*this, data, num_entries);
}


template<class T>
void Vector<T>::write(CommandQueue &queue, const std::vector<T> &data){
	queue.write(*this, data);
}


}
