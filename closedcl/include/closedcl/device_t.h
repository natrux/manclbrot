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
	size_t max_work_group_size;
	std::vector<size_t> max_work_item_sizes;
	cl_device_fp_config double_fp_config;

	static device_t from_id(cl_device_id device_id);
	static std::vector<device_t> find(const platform_t &platform, cl_device_type type=CL_DEVICE_TYPE_ALL);
	static device_t find(const platform_t &platform, const std::string &name);

	bool has_double() const;
};


}
