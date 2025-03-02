// GPbase.h : Include file for standard system include files,
// or project specific include files.

#pragma once
// Ensures GLFW won't include the development environment header. This prevents conflicts upon inclusion of gl3w .
#define SDL_MAIN_USE_CALLBACKS 1 // uses callbacks instead of the main as an entry point

#include <stdio.h>

// Window creation library. Also handles other things like input.
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
// C compatible math library for OpenGL
#include "cglm/cglm.h"
#include <vulkan/vulkan.h>
// Custom defines
// Global variables
// Bad practice but useful for this application.
#define OPENGL_VERSION_MAJOR 4
#define OPENGL_VERSION_MINOR 6


#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

 