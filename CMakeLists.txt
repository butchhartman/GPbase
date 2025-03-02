# CMakeList.txt : CMake project for GPbase, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GPbase")

# Add source to the gl3w library to be generated 
# This step only needs to be done once to generate the library. The source file for the gl3w library has since been removed from the project.
# gl3w will need to be recompiled and added in order to generate this library again.
# add_library (LoadShaders STATIC "src/LoadShaders.cpp")

# Add extern subdir
add_subdirectory(extern)

# Add link director(y/ies) for libraries
link_directories ("lib")

# Finds the Vulkan directory ? (Not actually sure how it works, research needed)
find_package(Vulkan REQUIRED)

# Add source to this project's executable.
add_executable (GPbase "src/sdlMain.c"  "include/GPbase.h" "include/SDL3/SDL.h")

# Makes directories in the source directory visible as system directories (can be included by <>)
# This allows libraries to work without me changing their include paths
target_include_directories (GPbase SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/include")

# TODO : Include cglm in project files
# Add libraries to this project's executable.
# Simply can specify the name of the library because CMake knows to look in the lib folder.
target_link_libraries (GPbase cglm SDL3 Vulkan::Vulkan) # <- Since vulkan is found by find_package, can put here (dont know what the :: are for...)

# Copies the pre-compiled SDL3.dll to the EXE's directory. This allows the program to funciton find the DLL AND is distributable.
# Note : This only occurs ONE TIME when the CMake generation is called. If it is removed, the program will fail to run.
file(COPY "lib/SDL3.dll"
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GPbase PROPERTY C_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.
