@echo off

ECHO Finding Vulkan SDK path...

IF "%VK_SDK_PATH%"==""  (
ECHO Vulkan SDK Path not found
EXIT
)

IF NOT "%VK_SDK_PATH%"=="" ECHO Vulkan SDK Path found

ECHO %VK_SDK_PATH%

ECHO Searching Vulkan SDK binary dir for glslc.exe...

IF NOT EXIST %VK_SDK_PATH%/Bin/glslc.exe EXIT

ECHO glslc.exe found
ECHO Compiling shaders...

%VK_SDK_PATH%/Bin/glslc.exe triangles.vert -o vert.spv
%VK_SDK_PATH%/Bin/glslc.exe triangles.frag -o frag.spv

ECHO Compilation finished

pause