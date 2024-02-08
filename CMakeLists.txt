cmake_minimum_required(VERSION 3.10)
project(Engine VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(
	Engine/Source/Dependencies/MikkTSpace
	Engine/Source/Dependencies/DirectXTex
	Engine/Source/Dependencies/SDL-2.0.16/include
    Engine/Source/Dependencies/GLEW-2.1.0/include/GL
    Engine/Source/Dependencies/ImGui-1.90-docking/include
	Engine/Source/Dependencies/ImGui-1.90-docking/include/backends
    Engine/Source/Dependencies/MathGeoLib-v1.5/include
	Engine/Source/Dependencies/MathGeoLib-v1.5/include/Math
	Engine/Source/Dependencies/Tinygltf-2.8.19
    Engine/Source/Dependencies/Physfs-3.2.0/include	
	Engine/Source/Dependencies/DebugDraw
    Engine/Source/Dependencies/RapidJSON-1.1.0/include
)

# Add header files
file(GLOB HEADERS "Engine/Source/*.h")

# Add source files
file(GLOB SOURCES "Engine/Source/*.cpp")

# Add executable
add_executable(Engine ${HEADERS} ${SOURCES})

# Add library dependencies
target_link_libraries(Engine
	Engine/Source/Dependencies/SDL-2.0.16/lib/x64
	Engine/Source/Dependencies/GLEW-2.1.0/lib/Release/x64
	Engine/Source/Dependencies/Physfs-3.2.0/lib/Debug
	Engine/Source/Dependencies/DirectXTex/lib/x64/Debug
)

# Install executable
#install(TARGETS my_executable DESTINATION bin)
