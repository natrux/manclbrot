#pragma once

#include <closedcl/Kernel.h>
#include <closedcl/device_t.h>

#include <string>
#include <vector>
#include <memory>

#include <CL/cl.h>


namespace closedcl{


class Program{
public:
	Program(cl_program program);
	~Program();
	Program(const Program &other) = delete;
	Program &operator=(const Program &other) = delete;

	void build(const std::vector<std::string> &options, const std::vector<device_t> &devices);
	std::shared_ptr<Kernel> create_kernel(const std::string &name) const;

private:
	cl_program program;
};


}
