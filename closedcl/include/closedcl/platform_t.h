#pragma once

#include <string>
#include <vector>

#include <CL/cl.h>


namespace closedcl{


struct platform_t{
	cl_platform_id id;
	std::string name;
	std::string version;

	static std::vector<platform_t> find();
	static platform_t choose();
};


}
