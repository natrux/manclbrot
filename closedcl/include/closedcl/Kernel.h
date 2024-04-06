#pragma once

#include <closedcl/Buffer.h>

#include <string>
#include <map>

#include <CL/cl.h>


namespace closedcl{


class Kernel{
public:
	Kernel(cl_kernel kernel);
	~Kernel();
	Kernel(const Kernel &other) = delete;
	Kernel &operator=(const Kernel &other) = delete;

	void set_argument(cl_uint index, cl_char value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_uchar value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_short value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_ushort value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_int value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_uint value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_long value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_ulong value){ set_argument_internal(index, value); }
	//void set_argument(cl_uint index, cl_half value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_float value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, cl_double value){ set_argument_internal(index, value); }
	void set_argument(cl_uint index, const Buffer &buffer){ set_argument_internal(index, buffer.data()); }
	template<class T>
	void set_argument(const std::string &name, const T &value){
		set_argument(find_argument(name), value);
	}

	cl_kernel data() const;

private:
	cl_kernel kernel;
	std::map<std::string, cl_uint> arguments;

	cl_uint find_argument(const std::string &name) const;
	void set_argument_internal(cl_uint index, size_t size, const void *value);
	template<class T>
	void set_argument_internal(cl_uint index, const T &value){
		set_argument_internal(index, sizeof(T), &value);
	}
};


}
