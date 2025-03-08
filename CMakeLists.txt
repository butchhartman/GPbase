cmake_minimum_required(VERSION 3.8)

project(GPbase)

#[[ 
	Makes CMake output compiled vendor binaries directly to the
	project's binary directory. This is useful because files like
	.DLL's need to be in the same directory as the executable 
	for the executable to run properly
]]
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

#[[ 
	Checks if the SDL3 directory contains a CMakeLists.txt file.
	If the CMakeLists.txt file does not exist, then it will run the git command
	to pull from the SDL3 repository and retrieve all its files.
]]
if(NOT "vendored/SDL3/CMakeLists.txt")
	execute_process(COMMAND git submodule update --init --recursive)
endif()

#[[ 
	Adds the vendored SDL3 directory as a subdirectory.
	Essentially, this means that the CMakeLists.txt file
	in the vendored/SDL3 directory will run.
]]
add_subdirectory("vendored/SDL3")

#[[ 
	Lets CMake know this project will compile to an executable
	comprised of the listed source files.
	TODO : Find a way to not have to include the file directory before the file name.
]]
add_executable(${PROJECT_NAME}  "src/sdlMain.c"  
								"include/GPbase.h" 
								"include/ValidationLayers.h" 
								"src/ValidationLayers.c" 
								"src/LoadShaders.c"  
								"include/LoadShaders.h")

#[[ 
	Finds the location of the host machine's Vulkan SDK
	The method by which it does this is not clear to me but
	I think it is based on environment variables. The required flag
	simply means that the project will not compile without it.
]]
find_package(Vulkan REQUIRED)

#[[ 
	Links Vulkan and vendored libraries against the executable.
	I am not sure where exactly these names come from (especially the vendored names)
	or what the *:: syntax's purpose is.
]]
target_link_libraries(${PROJECT_NAME} SDL3::SDL3 Vulkan::Vulkan)

#[[ 
	Adds the project's include directory and vendored libraries' include directory
	as system directories. This means that their header files can be included using
	<> and do not need the full path specified.
]]
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/vendored/SDL3/include/SDL3")