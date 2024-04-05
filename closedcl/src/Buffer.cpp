#include <closedcl/Buffer.h>
#include <closedcl/CommandQueue.h>


namespace closedcl{


Buffer::Buffer(cl_mem buffer_, size_t num_bytes_):
	buffer(buffer_),
	m_num_bytes(num_bytes_)
{
}


Buffer::~Buffer(){
	clReleaseMemObject(buffer);
}


void Buffer::read(CommandQueue &queue, void *data) const{
	queue.read(*this, data);
}


void Buffer::write(CommandQueue &queue, const void *data){
	queue.write(*this, data);
}


size_t Buffer::num_bytes() const{
	return m_num_bytes;
}


cl_mem Buffer::data() const{
	return buffer;
}


}
