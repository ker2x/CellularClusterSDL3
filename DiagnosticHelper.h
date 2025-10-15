#pragma once

#include <SDL3/SDL.h>
#include <CL/cl.h>

/**
 * Utility class for printing SDL and OpenCL diagnostic information.
 */
class DiagnosticHelper {
public:
    /// Print SDL version information
    static void printSDLVersion();

    /// Print available SDL video and render drivers
    static void printSDLRenderDrivers();

    /// Print detailed renderer properties and capabilities
    static void printSDLRendererInfo(SDL_Renderer* renderer);

    /// Print OpenCL platform and device information
    static void printOpenCLInfo();
};
