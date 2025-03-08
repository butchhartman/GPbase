cmake_minimum_required(VERSION 3.8)

project(GPbase)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(${PROJECT_NAME}  "src/sdlMain.c"  
								"include/GPbase.h" 
								"include/ValidationLayers.h" 
								"src/ValidationLayers.c" 
								"src/LoadShaders.c"  
								"include/LoadShaders.h")

if(NOT "vendored/SDL3/CMakeLists.txt")
	#SDl3 files do not exist, clone them.
	execute_process(COMMAND git submodule update --init --recursive)
endif()

add_subdirectory("vendored/SDL3")

find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} SDL3::SDL3 Vulkan::Vulkan)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/vendored/SDL3/include/SDL3")