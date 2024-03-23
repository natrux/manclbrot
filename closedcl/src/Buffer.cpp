#include <closedcl/Buffer.h>
#include <closedcl/CommandQueue.h>


namespace closedcl{


Buffer::Buffer(cl_mem buffer_):
	buffer(buffer_)
{
}


Buffer::~Buffer(){
	clReleaseMemObject(buffer);
}


void Buffer::read(CommandQueue &queue, void *data, size_t size) const{
	queue.read(*this, data, size);
}


void Buffer::write(CommandQueue &queue, const void *data, size_t size){
	queue.write(*this, data, size);
}


cl_mem Buffer::data() const{
	return buffer;
}


}
