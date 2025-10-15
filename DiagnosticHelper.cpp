#include "DiagnosticHelper.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

// Print available SDL video and render drivers
void DiagnosticHelper::printSDLRenderDrivers() {
    SDL_Log("=== SDL Video/Renderer Drivers ===");

    // Display current active video driver
    if (const char *video_driver = SDL_GetCurrentVideoDriver()) {
        SDL_Log("Current video driver: %s", video_driver);
    }

    // Enumerate all available render drivers
    const int num = SDL_GetNumRenderDrivers();
    if (num < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "SDL_GetNumRenderDrivers error: %s", SDL_GetError());
        return;
    }
    SDL_Log("Available render drivers: %d", num);
    for (int i = 0; i < num; ++i) {
        const char *name = SDL_GetRenderDriver(i);
        SDL_Log("  [%d] %s", i, (name ? name : "(unknown)"));
    }
}

// Helper function to convert SDL_Colorspace enum to human-readable string
static const char* getColorspaceName(SDL_Colorspace cs) {
    switch (cs) {
        case SDL_COLORSPACE_UNKNOWN: return "Unknown";
        case SDL_COLORSPACE_SRGB: return "sRGB (gamma corrected)";
        case SDL_COLORSPACE_SRGB_LINEAR: return "sRGB Linear";
        case SDL_COLORSPACE_HDR10: return "HDR10 (BT.2020 PQ)";
        case SDL_COLORSPACE_JPEG: return "JPEG (BT.709 YCbCr Full)";
        case SDL_COLORSPACE_BT601_LIMITED: return "BT.601 Limited";
        case SDL_COLORSPACE_BT601_FULL: return "BT.601 Full";
        case SDL_COLORSPACE_BT709_LIMITED: return "BT.709 Limited";
        case SDL_COLORSPACE_BT709_FULL: return "BT.709 Full";
        case SDL_COLORSPACE_BT2020_LIMITED: return "BT.2020 Limited";
        case SDL_COLORSPACE_BT2020_FULL: return "BT.2020 Full";
        default: return "Custom/Unknown";
    }
}

// Print detailed renderer properties and capabilities
void DiagnosticHelper::printSDLRendererInfo(SDL_Renderer *r) {
    SDL_Log("=== SDL Current Renderer ===");
    if (!r) { SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "No renderer"); return; }

    const char *name = SDL_GetRendererName(r);
    SDL_Log("Name: %s", (name ? name : "(unknown)"));

    int w = 0, h = 0;
    if (SDL_GetRenderOutputSize(r, &w, &h)) {
        SDL_Log("Output size: %dx%d", w, h);
    }

    // Query renderer properties for detailed capabilities
    if (SDL_PropertiesID props = SDL_GetRendererProperties(r)) {
        // Helper lambdas for property access
        auto getNum = [&](const char* key, Sint64 def=0){ return SDL_GetNumberProperty(props, key, def); };
        auto getStr = [&](const char* key, const char* def=""){ return SDL_GetStringProperty(props, key, def); };
        auto getPtr = [&](const char* key){ return SDL_GetPointerProperty(props, key, nullptr); };
        auto getBool = [&](const char* key, bool def=false){ return SDL_GetBooleanProperty(props, key, def); };

        // Display core renderer capabilities
        SDL_Log("Max texture size: %lld", (long long)getNum(SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, -1));
        SDL_Log("Driver (property): %s", getStr(SDL_PROP_RENDERER_NAME_STRING, "(unknown)"));
        SDL_Log("VSync setting:    %lld", (long long)getNum(SDL_PROP_RENDERER_VSYNC_NUMBER, 0));

        // Display colorspace in human-readable format
        SDL_Colorspace colorspace = static_cast<SDL_Colorspace>(getNum(SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER, 0));
        SDL_Log("Output colorspace: %s", getColorspaceName(colorspace));
        SDL_Log("HDR enabled:      %s", (getBool(SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, false) ? "Yes" : "No"));
        SDL_Log("SDR white point:  %f", SDL_GetFloatProperty(props, SDL_PROP_RENDERER_SDR_WHITE_POINT_FLOAT, 0.0f));
        SDL_Log("HDR headroom:     %f", SDL_GetFloatProperty(props, SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT, 0.0f));

        // List supported texture formats
        const SDL_PixelFormat *fmts = static_cast<const SDL_PixelFormat*>(getPtr(SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER));
        if (fmts) {
            SDL_Log("Texture formats:");
            bool found = false;
            for (int i = 0; fmts[i] != SDL_PIXELFORMAT_UNKNOWN; ++i) {
                SDL_Log("  - %s", SDL_GetPixelFormatName(fmts[i]));
                found = true;
            }
            if (!found) SDL_Log("  (none)");
        } else {
            SDL_Log("Texture formats:  (unknown)");
        }
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "No renderer properties available");
    }
}

// Print OpenCL platform and device information
void DiagnosticHelper::printOpenCLInfo() {
    SDL_Log("=== OpenCL Platforms and Devices ===");

    // Enumerate available OpenCL platforms
    cl_uint num_platforms = 0;
    cl_int err = clGetPlatformIDs(0, nullptr, &num_platforms);
    if (err != CL_SUCCESS) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "clGetPlatformIDs failed: %d", err);
        return;
    }

    std::vector<cl_platform_id> platforms(num_platforms);
    if (num_platforms > 0) clGetPlatformIDs(num_platforms, platforms.data(), nullptr);

    // Iterate through each platform
    for (cl_uint p = 0; p < num_platforms; ++p) {
        auto pid = platforms[p];

        // Helper lambda to query platform info strings
        auto getPlatStr = [&](cl_platform_info param) {
            size_t sz = 0; clGetPlatformInfo(pid, param, 0, nullptr, &sz);
            std::string s(sz, '\0');
            if (sz) clGetPlatformInfo(pid, param, sz, s.data(), nullptr);
            if (!s.empty() && s.back() == '\0') s.pop_back();
            return s;
        };

        // Display platform details
        SDL_Log("Platform [%u]", p);
        SDL_Log("  Profile:  %s", getPlatStr(CL_PLATFORM_PROFILE).c_str());
        SDL_Log("  Version:  %s", getPlatStr(CL_PLATFORM_VERSION).c_str());
        SDL_Log("  Name:     %s", getPlatStr(CL_PLATFORM_NAME).c_str());
        SDL_Log("  Vendor:   %s", getPlatStr(CL_PLATFORM_VENDOR).c_str());
        // Display platform extensions in readable format
        std::string extStr = getPlatStr(CL_PLATFORM_EXTENSIONS);
        SDL_Log("  Extensions:");
        size_t pos = 0, nextPos;
        while ((nextPos = extStr.find(' ', pos)) != std::string::npos) {
            std::string ext = extStr.substr(pos, nextPos - pos);
            if (!ext.empty()) SDL_Log("    - %s", ext.c_str());
            pos = nextPos + 1;
        }
        if (pos < extStr.size()) {
            std::string ext = extStr.substr(pos);
            if (!ext.empty()) SDL_Log("    - %s", ext.c_str());
        }

        // Enumerate devices for this platform
        cl_uint num_devices = 0;
        err = clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices);
        if (err != CL_SUCCESS) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "  clGetDeviceIDs failed: %d", err);
            continue;
        }
        std::vector<cl_device_id> devices(num_devices);
        if (num_devices > 0) clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, num_devices, devices.data(), nullptr);

        // Iterate through each device
        for (cl_uint d = 0; d < num_devices; ++d) {
            auto did = devices[d];

            // Helper lambdas to query device info
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

            // Display device details
            SDL_Log("  Device [%u]", d);
            SDL_Log("    Name:               %s", getDevStr(CL_DEVICE_NAME).c_str());
            SDL_Log("    Vendor:             %s", getDevStr(CL_DEVICE_VENDOR).c_str());
            SDL_Log("    Version:            %s", getDevStr(CL_DEVICE_VERSION).c_str());
            SDL_Log("    Driver version:     %s", getDevStr(CL_DRIVER_VERSION).c_str());
            std::string typeStr;
            if (dtype & CL_DEVICE_TYPE_CPU) typeStr += "CPU ";
            if (dtype & CL_DEVICE_TYPE_GPU) typeStr += "GPU ";
            if (dtype & CL_DEVICE_TYPE_ACCELERATOR) typeStr += "ACCELERATOR ";
            if (dtype & CL_DEVICE_TYPE_DEFAULT) typeStr += "DEFAULT ";
            SDL_Log("    Type:               %s", typeStr.c_str());
            SDL_Log("    Compute Units:      %u", getDevUInt(CL_DEVICE_MAX_COMPUTE_UNITS));
            SDL_Log("    Clock Frequency:    %u MHz", getDevUInt(CL_DEVICE_MAX_CLOCK_FREQUENCY));
            SDL_Log("    Global Mem:         %llu MiB", (unsigned long long)(getDevULong(CL_DEVICE_GLOBAL_MEM_SIZE) / (1024ULL * 1024ULL)));
            SDL_Log("    Local Mem:          %llu KiB", (unsigned long long)(getDevULong(CL_DEVICE_LOCAL_MEM_SIZE) / 1024ULL));
            SDL_Log("    Max WG Size:        %zu", getDevSizeT(CL_DEVICE_MAX_WORK_GROUP_SIZE));
            {
                size_t dims[3] = {0,0,0};
                size_t sz = 0; clGetDeviceInfo(did, CL_DEVICE_MAX_WORK_ITEM_SIZES, 0, nullptr, &sz);
                if (sz >= sizeof(dims)) {
                    clGetDeviceInfo(did, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(dims), dims, nullptr);
                    SDL_Log("    Max WG Dim:         %zux%zux%zu", dims[0], dims[1], dims[2]);
                } else {
                    SDL_Log("    Max WG Dim:         n/a");
                }
            }
            SDL_Log("    Image Support:      %s", (getDevUInt(CL_DEVICE_IMAGE_SUPPORT) ? "Yes" : "No"));

            // Display device extensions in readable format
            std::string devExtStr = getDevStr(CL_DEVICE_EXTENSIONS);
            SDL_Log("    Extensions:");
            size_t devPos = 0, devNextPos;
            while ((devNextPos = devExtStr.find(' ', devPos)) != std::string::npos) {
                std::string ext = devExtStr.substr(devPos, devNextPos - devPos);
                if (!ext.empty()) SDL_Log("      - %s", ext.c_str());
                devPos = devNextPos + 1;
            }
            if (devPos < devExtStr.size()) {
                std::string ext = devExtStr.substr(devPos);
                if (!ext.empty()) SDL_Log("      - %s", ext.c_str());
            }
        }
    }
}
