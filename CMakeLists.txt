cmake_minimum_required(VERSION 3.16)
project(manclbrot CXX)

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
add_definitions(-D CL_TARGET_OPENCL_VERSION=120)

add_subdirectory(wideopencl)

find_package(SDL2 REQUIRED)


add_executable(manclbrot
	src/MandelbrotRendererInterface.cpp
	src/MandelbrotRenderer.cpp
	src/CL_MandelbrotRenderer.cpp
	src/main.cpp
)

target_include_directories(manclbrot PUBLIC
	include
)

target_link_libraries(manclbrot
	wideopencl
	${SDL2_LIBRARIES}
)


