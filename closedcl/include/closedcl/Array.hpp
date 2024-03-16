#pragma once

#include <closedcl/Array.h>
#include <closedcl/CommandQueue.h>


namespace closedcl{


template<class T, size_t N>
void Array<T, N>::read(CommandQueue &queue, std::array<T, N> &data){
	queue.read(*this, data);
}


template<class T, size_t N>
void Array<T, N>::write(CommandQueue &queue, const std::array<T, N> &data){
	queue.write(*this, data);
}


}
