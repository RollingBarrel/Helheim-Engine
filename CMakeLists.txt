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
	${OPENGL_INCLUDE_DIR}
	${GLU_INCLUDE_DIR}
)

find_package(OpenGL REQUIRED)
find_package(GLU REQUIRED)


# Add header files
file(GLOB HEADERS "Engine/Source/*.h")

# Add source files
file(GLOB SOURCES "Engine/Source/*.cpp")

# Add executable
add_executable(Engine ${HEADERS} ${SOURCES})

# Add library dependencies
target_link_libraries(Engine
	SDL2
	SDL2main
	glew32
	opengl32
	physfs
	DirectXTex
	${OPENGL_LIBRARIES}
	${GLU_LIBRARY}
)

# Install executable
#install(TARGETS my_executable DESTINATION bin)
