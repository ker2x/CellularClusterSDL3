cmake_minimum_required(VERSION 3.20)
project(CellularClusterSDL3)

set(CMAKE_CXX_STANDARD 23)

# Find SDL3
find_package(SDL3 REQUIRED CONFIG)
find_package(OpenCL REQUIRED)

# add main
add_executable(main
    main.cpp
    App.cpp
    FPSCounter.cpp
)

# Link SDL3 to the executable
target_link_libraries(main PRIVATE SDL3::SDL3 OpenCL::OpenCL)

# Explicitly target OpenCL 3.0 to silence cl_version.h note
# See cl_version.h: define CL_TARGET_OPENCL_VERSION in 100 x major + 10 x minor form
# 300 = OpenCL 3.0
# Adjust here if you need to target a different minimum OpenCL C API level.
target_compile_definitions(main PRIVATE CL_TARGET_OPENCL_VERSION=300)
