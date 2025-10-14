#include "DiagnosticHelper.h"

#include <iostream>
#include <vector>
#include <iomanip>

// Print available SDL video and render drivers
void DiagnosticHelper::printSDLRenderDrivers() {
    std::cout << "=== SDL Video/Renderer Drivers ===" << std::endl;
    if (const char *video_driver = SDL_GetCurrentVideoDriver()) {
        std::cout << "Current video driver: " << video_driver << std::endl;
    }

    const int num = SDL_GetNumRenderDrivers();
    if (num < 0) {
        std::cout << "SDL_GetNumRenderDrivers error: " << SDL_GetError() << std::endl;
        return;
    }
    std::cout << "Available render drivers: " << num << std::endl;
    for (int i = 0; i < num; ++i) {
        const char *name = SDL_GetRenderDriver(i);
        std::cout << "  [" << i << "] " << (name ? name : "(unknown)") << std::endl;
    }
}

// Print detailed renderer properties and capabilities
void DiagnosticHelper::printSDLRendererInfo(SDL_Renderer *r) {
    std::cout << "=== SDL Current Renderer ===" << std::endl;
    if (!r) { std::cout << "No renderer" << std::endl; return; }

    const char *name = SDL_GetRendererName(r);
    std::cout << "Name: " << (name ? name : "(unknown)") << std::endl;

    int w = 0, h = 0;
    if (SDL_GetRenderOutputSize(r, &w, &h)) {
        std::cout << "Output size: " << w << "x" << h << std::endl;
    }

    if (SDL_PropertiesID props = SDL_GetRendererProperties(r)) {
        auto getNum = [&](const char* key, Sint64 def=0){ return SDL_GetNumberProperty(props, key, def); };
        auto getStr = [&](const char* key, const char* def=""){ return SDL_GetStringProperty(props, key, def); };
        auto getPtr = [&](const char* key){ return SDL_GetPointerProperty(props, key, nullptr); };
        auto getBool = [&](const char* key, bool def=false){ return SDL_GetBooleanProperty(props, key, def); };

        std::cout << "Max texture size: " << getNum(SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, -1) << std::endl;
        std::cout << "Driver (property): " << getStr(SDL_PROP_RENDERER_NAME_STRING, "(unknown)") << std::endl;
        std::cout << "VSync setting:    " << getNum(SDL_PROP_RENDERER_VSYNC_NUMBER, 0) << std::endl;
        std::cout << "Output colorspace:" << getNum(SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER, 0) << std::endl;
        std::cout << "HDR enabled:      " << (getBool(SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, false) ? "Yes" : "No") << std::endl;
        std::cout << "SDR white point:  " << SDL_GetFloatProperty(props, SDL_PROP_RENDERER_SDR_WHITE_POINT_FLOAT, 0.0f) << std::endl;
        std::cout << "HDR headroom:     " << SDL_GetFloatProperty(props, SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT, 0.0f) << std::endl;

        const SDL_PixelFormat *fmts = static_cast<const SDL_PixelFormat*>(getPtr(SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER));
        std::cout << "Texture formats:  ";
        if (fmts) {
            bool first = true;
            for (int i = 0; fmts[i] != SDL_PIXELFORMAT_UNKNOWN; ++i) {
                if (!first) std::cout << ", ";
                first = false;
                std::cout << SDL_GetPixelFormatName(fmts[i]);
            }
            if (first) std::cout << "(none)";
            std::cout << std::endl;
        } else {
            std::cout << "(unknown)" << std::endl;
        }
    } else {
        std::cout << "No renderer properties available" << std::endl;
    }
}

// Print OpenCL platform and device information
void DiagnosticHelper::printOpenCLInfo() {
    std::cout << "=== OpenCL Platforms and Devices ===" << std::endl;

    cl_uint num_platforms = 0;
    cl_int err = clGetPlatformIDs(0, nullptr, &num_platforms);
    if (err != CL_SUCCESS) {
        std::cout << "clGetPlatformIDs failed: " << err << std::endl;
        return;
    }

    std::vector<cl_platform_id> platforms(num_platforms);
    if (num_platforms > 0) clGetPlatformIDs(num_platforms, platforms.data(), nullptr);

    for (cl_uint p = 0; p < num_platforms; ++p) {
        auto pid = platforms[p];
        auto getPlatStr = [&](cl_platform_info param) {
            size_t sz = 0; clGetPlatformInfo(pid, param, 0, nullptr, &sz);
            std::string s(sz, '\0');
            if (sz) clGetPlatformInfo(pid, param, sz, s.data(), nullptr);
            if (!s.empty() && s.back() == '\0') s.pop_back();
            return s;
        };
        std::cout << "Platform [" << p << "]" << std::endl;
        std::cout << "  Profile:  " << getPlatStr(CL_PLATFORM_PROFILE) << std::endl;
        std::cout << "  Version:  " << getPlatStr(CL_PLATFORM_VERSION) << std::endl;
        std::cout << "  Name:     " << getPlatStr(CL_PLATFORM_NAME) << std::endl;
        std::cout << "  Vendor:   " << getPlatStr(CL_PLATFORM_VENDOR) << std::endl;
        std::cout << "  Extensions: " << getPlatStr(CL_PLATFORM_EXTENSIONS) << std::endl;

        cl_uint num_devices = 0;
        err = clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices);
        if (err != CL_SUCCESS) {
            std::cout << "  clGetDeviceIDs failed: " << err << std::endl;
            continue;
        }
        std::vector<cl_device_id> devices(num_devices);
        if (num_devices > 0) clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, num_devices, devices.data(), nullptr);

        for (cl_uint d = 0; d < num_devices; ++d) {
            auto did = devices[d];
            auto getDevStr = [&](cl_device_info param) {
                size_t sz = 0; clGetDeviceInfo(did, param, 0, nullptr, &sz);
                std::string s(sz, '\0');
                if (sz) clGetDeviceInfo(did, param, sz, s.data(), nullptr);
                if (!s.empty() && s.back() == '\0') s.pop_back();
                return s;
            };
            auto getDevULong = [&](cl_device_info param) {
                cl_ulong v = 0; clGetDeviceInfo(did, param, sizeof(v), &v, nullptr); return v; };
            auto getDevUInt = [&](cl_device_info param) {
                cl_uint v = 0; clGetDeviceInfo(did, param, sizeof(v), &v, nullptr); return v; };
            auto getDevSizeT = [&](cl_device_info param) {
                size_t v = 0; clGetDeviceInfo(did, param, sizeof(v), &v, nullptr); return v; };
            cl_device_type dtype = 0; clGetDeviceInfo(did, CL_DEVICE_TYPE, sizeof(dtype), &dtype, nullptr);

            std::cout << "  Device [" << d << "]" << std::endl;
            std::cout << "    Name:               " << getDevStr(CL_DEVICE_NAME) << std::endl;
            std::cout << "    Vendor:             " << getDevStr(CL_DEVICE_VENDOR) << std::endl;
            std::cout << "    Version:            " << getDevStr(CL_DEVICE_VERSION) << std::endl;
            std::cout << "    Driver version:     " << getDevStr(CL_DRIVER_VERSION) << std::endl;
            std::cout << "    Type:               "
                      << ((dtype & CL_DEVICE_TYPE_CPU) ? "CPU " : "")
                      << ((dtype & CL_DEVICE_TYPE_GPU) ? "GPU " : "")
                      << ((dtype & CL_DEVICE_TYPE_ACCELERATOR) ? "ACCELERATOR " : "")
                      << ((dtype & CL_DEVICE_TYPE_DEFAULT) ? "DEFAULT " : "")
                      << std::endl;
            std::cout << "    Compute Units:      " << getDevUInt(CL_DEVICE_MAX_COMPUTE_UNITS) << std::endl;
            std::cout << "    Clock Frequency:    " << getDevUInt(CL_DEVICE_MAX_CLOCK_FREQUENCY) << " MHz" << std::endl;
            std::cout << "    Global Mem:         " << (getDevULong(CL_DEVICE_GLOBAL_MEM_SIZE) / (1024 * 1024)) << " MiB" << std::endl;
            std::cout << "    Local Mem:          " << (getDevULong(CL_DEVICE_LOCAL_MEM_SIZE) / 1024) << " KiB" << std::endl;
            std::cout << "    Max WG Size:        " << getDevSizeT(CL_DEVICE_MAX_WORK_GROUP_SIZE) << std::endl;
            std::cout << "    Max WG Dim:         ";
            {
                size_t dims[3] = {0,0,0};
                size_t sz = 0; clGetDeviceInfo(did, CL_DEVICE_MAX_WORK_ITEM_SIZES, 0, nullptr, &sz);
                if (sz >= sizeof(dims)) {
                    clGetDeviceInfo(did, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(dims), dims, nullptr);
                    std::cout << dims[0] << "x" << dims[1] << "x" << dims[2] << std::endl;
                } else {
                    std::cout << "n/a" << std::endl;
                }
            }
            std::cout << "    Image Support:      " << (getDevUInt(CL_DEVICE_IMAGE_SUPPORT) ? "Yes" : "No") << std::endl;
            std::cout << "    Extensions:         " << getDevStr(CL_DEVICE_EXTENSIONS) << std::endl;
        }
    }
}
