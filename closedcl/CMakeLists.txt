cmake_minimum_required(VERSION 3.16)
project(closedcl CXX)

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(
		-pedantic -Wall -Wextra
		-Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization
		-Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations
		-Wmissing-include-dirs -Wold-style-cast -Wredundant-decls -Wshadow
		-Wsign-conversion -Wsign-promo -Wstrict-null-sentinel
		-Wstrict-overflow -Wsuggest-override -Wswitch-default -Wundef

		#-Werror
	)
else()
	message(WARNING "Unknown compiler - not turning on warnings")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(OpenCL REQUIRED)

add_definitions(-D CL_TARGET_OPENCL_VERSION=120)
add_subdirectory(examples)



add_library(closedcl SHARED
	src/platform_t.cpp
	src/device_t.cpp
	src/Buffer.cpp
	src/CommandQueue.cpp
	src/Kernel.cpp
	src/Program.cpp
	src/Context.cpp
	src/error.cpp
)

target_include_directories(closedcl PUBLIC
	include
)

target_link_libraries(closedcl
	${OpenCL_LIBRARIES}
)


