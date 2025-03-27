// GPbase.h : Include file for standard system include files,
// or project specific include files.

#ifndef GP_BASE_INCLUDED_
#define GP_BASE_INCLUDED_
#pragma once
#define SDL_MAIN_USE_CALLBACKS 1 // uses callbacks instead of the main as an entry point

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480


//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

// Standard libraries
#include <stdio.h>
#include <string.h>

// SDL : Window creation, audio, input, and 2D graphics library
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_vulkan.h"

// CGLM : Graphics math library compatible with C
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include "cglm/struct.h"

// 3D graphics API
#include <vulkan/vulkan.h>

#define DEBUG_MODE
#include "Globals.h"

// Header containing functions involving validation layers
#include "ValidationLayers.h"

#include "LoadShaders.h"

#include "time.h"

#include "Buffers.h"

#include "Descriptors.h"

#include "Device.h"

#include "Instance.h"

#endif // GP_BASE_INCLUDED_