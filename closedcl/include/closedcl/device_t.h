#pragma once

#include <closedcl/platform_t.h>

#include <string>
#include <vector>

#include <CL/cl.h>


namespace closedcl{


struct device_t{
	cl_device_id id;
	cl_device_type type;
	std::string name;
	std::string version;
	cl_device_fp_config has_double;

	static std::vector<device_t> find(const platform_t &platform, cl_device_type type=CL_DEVICE_TYPE_ALL);
	static device_t find(const platform_t &platform, const std::string &name);
};


}
