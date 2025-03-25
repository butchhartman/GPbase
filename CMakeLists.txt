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
	Runs the git submodule update command to retrieve submodule project
	files.
]]
execute_process(COMMAND git submodule update --init --recursive)

#[[ 
	Adds the project directories in vendored as a subdirectory.
	This means their CMakeLists.txt files will run.
]]
add_subdirectory("vendored/SDL3")
add_subdirectory("vendored/cglm")

add_compile_options(/fsanitize=address /g3 /Wall)

#[[
	Gets all .c files from the src directory and stores them at ${PROJECT_SOURCE_FILES}
]]
file(GLOB_RECURSE PROJECT_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.c)

#[[ 
	Lets CMake know this project will compile to an executable
	comprised of the listed source files.
	TODO : Find a way to not have to include the file directory before the file name.
]]
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES} )

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
target_link_libraries(${PROJECT_NAME} SDL3::SDL3 Vulkan::Vulkan cglm)

#[[ 
	Adds the project's include directory and vendored libraries' include directory
	as system directories. This means that their header files can be included using
	<> and do not need the full path specified.
]]
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/vendored/SDL3/include/SDL3")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/vendored/cglm/include/cglm")