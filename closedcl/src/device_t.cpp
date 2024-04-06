#include <closedcl/device_t.h>
#include <closedcl/error.h>

#include <stdexcept>


namespace closedcl{


std::vector<device_t> device_t::find(const platform_t &platform, cl_device_type type){
	std::vector<cl_device_id> device_ids;
	{
		cl_uint num_device_ids = 0;
		const auto error = clGetDeviceIDs(platform.id, type, 0, NULL, &num_device_ids);
		if(error != CL_SUCCESS){
			throw std::runtime_error("clGetDeviceIDs() failed with: " + error_string(error));
		}
		device_ids.resize(num_device_ids);
	}
	{
		const auto error = clGetDeviceIDs(platform.id, type, device_ids.size(), device_ids.data(), NULL);
		if(error != CL_SUCCESS){
			throw std::runtime_error("clGetDeviceIDs() failed with: " + error_string(error));
		}
	}

	std::vector<device_t> result;
	for(const auto &device_id : device_ids){
		device_t device;
		device.id = device_id;

		{
			const auto error = clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(device.type), &device.type, NULL);
			if(error != CL_SUCCESS){
				throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_TYPE) failed with: " + error_string(error));
			}
		}

		{
			std::vector<char> device_name;
			{
				size_t device_name_size = 0;
				const auto error = clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &device_name_size);
				if(error != CL_SUCCESS){
					throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_NAME) failed with: " + error_string(error));
				}
				device_name.resize(device_name_size);
			}
			{
				const auto error = clGetDeviceInfo(device_id, CL_DEVICE_NAME, device_name.size(), device_name.data(), NULL);
				if(error != CL_SUCCESS){
					throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_NAME) failed with: " + error_string(error));
				}
			}
			device.name = std::string(device_name.begin(), device_name.end());
		}

		{
			std::vector<char> device_version;
			{
				size_t device_version_size = 0;
				const auto error = clGetDeviceInfo(device_id, CL_DEVICE_VERSION, 0, NULL, &device_version_size);
				if(error != CL_SUCCESS){
					throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_VERSION) failed with: " + error_string(error));
				}
				device_version.resize(device_version_size);
			}
			{
				const auto error = clGetDeviceInfo(device_id, CL_DEVICE_VERSION, device_version.size(), device_version.data(), NULL);
				if(error != CL_SUCCESS){
					throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_VERSION) failed with: " + error_string(error));
				}
			}
			device.version = std::string(device_version.begin(), device_version.end());
		}

		{
			const auto error = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(device.max_work_group_size), &device.max_work_group_size, NULL);
			if(error != CL_SUCCESS){
				throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE) failed with: " + error_string(error));
			}
		}

		{
			cl_uint max_work_item_dimensions;
			const auto error = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(max_work_item_dimensions), &max_work_item_dimensions, NULL);
			if(error != CL_SUCCESS){
				throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS) failed with: " + error_string(error));
			}
			device.max_work_item_sizes.resize(max_work_item_dimensions);
		}
		{
			const auto buffer = device.max_work_item_sizes.data();
			const auto size = device.max_work_item_sizes.size() * sizeof(decltype(device.max_work_item_sizes)::value_type);
			const auto error = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, size, buffer, NULL);
			if(error != CL_SUCCESS){
				throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES) failed with: " + error_string(error));
			}
		}

		{
			const auto error = clGetDeviceInfo(device_id, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(device.double_fp_config), &device.double_fp_config, NULL);
			if(error != CL_SUCCESS){
				throw std::runtime_error("clGetDeviceInfo(CL_DEVICE_DOUBLE_FP_CONFIG) failed with: " + error_string(error));
			}
		}

		result.push_back(device);
	}

	return result;
}


device_t device_t::find(const platform_t &platform, const std::string &name){
	for(const auto &device : find(platform)){
		if(device.name == name){
			return device;
		}
	}
	throw std::runtime_error("No such OpenCL device: '" + name + "'");
}


bool device_t::has_double() const{
	return (double_fp_config != 0);
}


}
