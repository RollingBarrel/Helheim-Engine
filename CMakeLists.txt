cmake_minimum_required(VERSION 3.10)
project(Engine VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(
    Engine/Source/Dependencies/GLEW-2.1.0/include
    Engine/Source/Dependencies/ImGui-1.90-docking/include
    Engine/Source/Dependencies/MathGeoLib-v1.5/include
    Engine/Source/Dependencies/Physfs-3.2.0/include
    Engine/Source/Dependencies/RapidJSON-1.1.0/include
    Engine/Source/Dependencies/SDL-2.0.16/include
)

# Add header files
file(GLOB HEADERS "Engine/Source/*.h")

# Add source files
file(GLOB SOURCES "Engine/Source/*.cpp")

# Add executable
add_executable(my_executable ${HEADERS} ${SOURCES})

# Add library dependencies
target_link_libraries(my_executable PRIVATE Engine/Game/*)

# Install executable
#install(TARGETS my_executable DESTINATION bin)
