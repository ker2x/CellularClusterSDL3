#include "DiagnosticHelper.h"

#include <vector>
#include <string>
#include <ranges>

// ============================================================================
// Helper Functions
// ============================================================================

namespace {
    // Split space-separated string into individual items using C++23 ranges
    std::vector<std::string> splitBySpace(const std::string &str) {
        auto words = str
            | std::views::split(' ')
            | std::views::filter([](auto&& word) { return !std::ranges::empty(word); })
            | std::views::transform([](auto&& word) {
                return std::string(word.begin(), word.end());
            });
        return std::ranges::to<std::vector>(words);
    }

    // Print a list of items with bullet points
    void printList(const std::string &title, const std::vector<std::string> &items, const char *indent = "  ") {
        SDL_Log("%s%s", indent, title.c_str());
        if (items.empty()) {
            SDL_Log("%s  (none)", indent);
        } else {
            for (const auto &item: items) {
                SDL_Log("%s  - %s", indent, item.c_str());
            }
        }
    }

    // Convert SDL_Colorspace enum to human-readable string
    const char *getColorspaceName(SDL_Colorspace cs) {
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

    // Query OpenCL platform string property
    std::string getClPlatformString(cl_platform_id platform, cl_platform_info param) {
        size_t size = 0;
        clGetPlatformInfo(platform, param, 0, nullptr, &size);
        std::string result(size, '\0');
        if (size > 0) {
            clGetPlatformInfo(platform, param, size, result.data(), nullptr);
            if (!result.empty() && result.back() == '\0') {
                result.pop_back();
            }
        }
        return result;
    }

    // Query OpenCL device string property
    std::string getClDeviceString(cl_device_id device, cl_device_info param) {
        size_t size = 0;
        clGetDeviceInfo(device, param, 0, nullptr, &size);
        std::string result(size, '\0');
        if (size > 0) {
            clGetDeviceInfo(device, param, size, result.data(), nullptr);
            if (!result.empty() && result.back() == '\0') {
                result.pop_back();
            }
        }
        return result;
    }

    // Query OpenCL device unsigned long property
    cl_ulong getClDeviceULong(cl_device_id device, cl_device_info param) {
        cl_ulong value = 0;
        clGetDeviceInfo(device, param, sizeof(value), &value, nullptr);
        return value;
    }

    // Query OpenCL device unsigned int property
    cl_uint getClDeviceUInt(cl_device_id device, cl_device_info param) {
        cl_uint value = 0;
        clGetDeviceInfo(device, param, sizeof(value), &value, nullptr);
        return value;
    }

    // Query OpenCL device size_t property
    size_t getClDeviceSizeT(cl_device_id device, cl_device_info param) {
        size_t value = 0;
        clGetDeviceInfo(device, param, sizeof(value), &value, nullptr);
        return value;
    }

    // Convert OpenCL device type flags to string
    std::string getDeviceTypeString(cl_device_type type) {
        std::string result;
        if (type & CL_DEVICE_TYPE_CPU) result += "CPU ";
        if (type & CL_DEVICE_TYPE_GPU) result += "GPU ";
        if (type & CL_DEVICE_TYPE_ACCELERATOR) result += "ACCELERATOR ";
        if (type & CL_DEVICE_TYPE_DEFAULT) result += "DEFAULT ";
        if (!result.empty()) result.pop_back(); // Remove trailing space
        return result;
    }
} // anonymous namespace

// ============================================================================
// SDL Diagnostics
// ============================================================================

void DiagnosticHelper::printSDLVersion() {
    SDL_Log("=== SDL Version ===");

    constexpr int compiled_major = SDL_MAJOR_VERSION;
    constexpr int compiled_minor = SDL_MINOR_VERSION;
    constexpr int compiled_patch = SDL_MICRO_VERSION;
    SDL_Log("Compiled against: %d.%d.%d", compiled_major, compiled_minor, compiled_patch);

    const int linked_version = SDL_GetVersion();
    const int linked_major = SDL_VERSIONNUM_MAJOR(linked_version);
    const int linked_minor = SDL_VERSIONNUM_MINOR(linked_version);
    const int linked_patch = SDL_VERSIONNUM_MICRO(linked_version);
    SDL_Log("Linked against:   %d.%d.%d", linked_major, linked_minor, linked_patch);

    const char* revision = SDL_GetRevision();
    if (revision && revision[0] != '\0') {
        SDL_Log("Revision:         %s", revision);
    }
}

void DiagnosticHelper::printSDLRenderDrivers() {
    SDL_Log("=== SDL Video/Renderer Drivers ===");

    // Current video driver
    if (const char *video_driver = SDL_GetCurrentVideoDriver()) {
        SDL_Log("Current video driver: %s", video_driver);
    }

    // Available render drivers
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

void DiagnosticHelper::printSDLRendererInfo(SDL_Renderer *renderer) {
    SDL_Log("=== SDL Current Renderer ===");

    if (!renderer) {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "No renderer");
        return;
    }

    // Basic renderer info
    const char *name = SDL_GetRendererName(renderer);
    SDL_Log("Name: %s", (name ? name : "(unknown)"));

    int width = 0, height = 0;
    if (SDL_GetRenderOutputSize(renderer, &width, &height)) {
        SDL_Log("Output size: %dx%d", width, height);
    }

    // Query renderer properties
    const SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
    if (!props) {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "No renderer properties available");
        return;
    }

    // Core capabilities
    SDL_Log("Max texture size: %lld",
            static_cast<long long>(SDL_GetNumberProperty(props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, -1)));
    SDL_Log("Driver:           %s",
            SDL_GetStringProperty(props, SDL_PROP_RENDERER_NAME_STRING, "(unknown)"));
    SDL_Log("VSync:            %lld",
            static_cast<long long>(SDL_GetNumberProperty(props, SDL_PROP_RENDERER_VSYNC_NUMBER, 0)));

    // Colorspace
    const SDL_Colorspace colorspace = static_cast<SDL_Colorspace>(
        SDL_GetNumberProperty(props, SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER, 0));
    SDL_Log("Colorspace:       %s", getColorspaceName(colorspace));

    // HDR info
    const bool hdrEnabled = SDL_GetBooleanProperty(props, SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, false);
    SDL_Log("HDR enabled:      %s", hdrEnabled ? "Yes" : "No");
    if (hdrEnabled) {
        SDL_Log("  SDR white point: %.2f",
                SDL_GetFloatProperty(props, SDL_PROP_RENDERER_SDR_WHITE_POINT_FLOAT, 0.0f));
        SDL_Log("  HDR headroom:    %.2f",
                SDL_GetFloatProperty(props, SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT, 0.0f));
    }

    // Texture formats
    const SDL_PixelFormat *formats = static_cast<const SDL_PixelFormat *>(
        SDL_GetPointerProperty(props, SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER, nullptr));

    if (formats) {
        std::vector<std::string> formatNames;
        for (int i = 0; formats[i] != SDL_PIXELFORMAT_UNKNOWN; ++i) {
            formatNames.emplace_back(SDL_GetPixelFormatName(formats[i]));
        }
        printList("Texture formats:", formatNames);
    } else {
        SDL_Log("Texture formats:  (unknown)");
    }
}

// ============================================================================
// OpenCL Diagnostics
// ============================================================================

namespace {
    void printClPlatform(cl_platform_id platform, cl_uint index) {
        SDL_Log("Platform [%u]", index);
        SDL_Log("  Profile:  %s", getClPlatformString(platform, CL_PLATFORM_PROFILE).c_str());
        SDL_Log("  Version:  %s", getClPlatformString(platform, CL_PLATFORM_VERSION).c_str());
        SDL_Log("  Name:     %s", getClPlatformString(platform, CL_PLATFORM_NAME).c_str());
        SDL_Log("  Vendor:   %s", getClPlatformString(platform, CL_PLATFORM_VENDOR).c_str());

        // Extensions
        std::string extensionsStr = getClPlatformString(platform, CL_PLATFORM_EXTENSIONS);
        std::vector<std::string> extensions = splitBySpace(extensionsStr);
        printList("Extensions:", extensions, "  ");
    }

    void printClDevice(cl_device_id device, cl_uint index) {
        SDL_Log("  Device [%u]", index);

        // Basic info
        SDL_Log("    Name:            %s", getClDeviceString(device, CL_DEVICE_NAME).c_str());
        SDL_Log("    Vendor:          %s", getClDeviceString(device, CL_DEVICE_VENDOR).c_str());
        SDL_Log("    Version:         %s", getClDeviceString(device, CL_DEVICE_VERSION).c_str());
        SDL_Log("    Driver version:  %s", getClDeviceString(device, CL_DRIVER_VERSION).c_str());

        // Device type
        cl_device_type type = 0;
        clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, nullptr);
        SDL_Log("    Type:            %s", getDeviceTypeString(type).c_str());

        // Compute capabilities
        SDL_Log("    Compute Units:   %u", getClDeviceUInt(device, CL_DEVICE_MAX_COMPUTE_UNITS));
        SDL_Log("    Clock Frequency: %u MHz", getClDeviceUInt(device, CL_DEVICE_MAX_CLOCK_FREQUENCY));

        // Memory info
        const cl_ulong globalMem = getClDeviceULong(device, CL_DEVICE_GLOBAL_MEM_SIZE);
        const cl_ulong localMem = getClDeviceULong(device, CL_DEVICE_LOCAL_MEM_SIZE);
        SDL_Log("    Global Memory:   %llu MiB", static_cast<unsigned long long>(globalMem / (1024ULL * 1024ULL)));
        SDL_Log("    Local Memory:    %llu KiB", static_cast<unsigned long long>(localMem / 1024ULL));

        // Work group info
        const size_t maxWorkGroupSize = getClDeviceSizeT(device, CL_DEVICE_MAX_WORK_GROUP_SIZE);
        SDL_Log("    Max WG Size:     %zu", maxWorkGroupSize);

        size_t dims[3] = {0, 0, 0};
        size_t size = 0;
        clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, 0, nullptr, &size);
        if (size >= sizeof(dims)) {
            clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(dims), dims, nullptr);
            SDL_Log("    Max WG Dims:     %zu x %zu x %zu", dims[0], dims[1], dims[2]);
        } else {
            SDL_Log("    Max WG Dims:     n/a");
        }

        // Image support
        const bool imageSupport = getClDeviceUInt(device, CL_DEVICE_IMAGE_SUPPORT) != 0;
        SDL_Log("    Image Support:   %s", imageSupport ? "Yes" : "No");

        // Extensions
        const std::string extensionsStr = getClDeviceString(device, CL_DEVICE_EXTENSIONS);
        const std::vector<std::string> extensions = splitBySpace(extensionsStr);
        printList("Extensions:", extensions, "    ");
    }
} // anonymous namespace

void DiagnosticHelper::printOpenCLInfo() {
    SDL_Log("=== OpenCL Platforms and Devices ===");

    // Get platforms
    cl_uint numPlatforms = 0;
    cl_int err = clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (err != CL_SUCCESS) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "clGetPlatformIDs failed: %d", err);
        return;
    }

    if (numPlatforms == 0) {
        SDL_Log("No OpenCL platforms found");
        return;
    }

    std::vector<cl_platform_id> platforms(numPlatforms);
    clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);

    // Iterate platforms
    for (cl_uint p = 0; p < numPlatforms; ++p) {
        printClPlatform(platforms[p], p);

        // Get devices for this platform
        cl_uint numDevices = 0;
        err = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
        if (err != CL_SUCCESS) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "  clGetDeviceIDs failed: %d", err);
            continue;
        }

        if (numDevices == 0) {
            SDL_Log("  No devices found");
            continue;
        }

        std::vector<cl_device_id> devices(numDevices);
        clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, numDevices, devices.data(), nullptr);

        // Iterate devices
        for (cl_uint d = 0; d < numDevices; ++d) {
            printClDevice(devices[d], d);
        }
    }
}
