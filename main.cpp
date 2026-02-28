#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

// #define VULKAN_HPP_NO_EXCEPTIONS
// Used to remove exceptions and retrieve errors as tuples like
// auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);
//
// if (result == vk::Result::eErrorOutOfDateKHR)
// {
//     recreateSwapChain();
//     return;
// }
// if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
// {
//     throw std::runtime_error("failed to acquire swap chain image!");
// }

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "GLFW/glfw3.h"
#include "dodo/core/App.hpp"

const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


// Create DodoApp that takes WindowsInfo = WindowsInfo::default and VulkanInfo = VulkanInfo = default
// Init windows and vulkan in constructor -> Problem no error in constructor
// -> constructor do nothing but add init function and cleanup automatically if initialized
// Cleanup in destructor -> Rule of 5 or delete move and copy;
// Function setDebug for validation layers and custom validation layers with default or custom callbacks

class HelloTriangleApplication {
private:
    GLFWwindow * _window = nullptr;
    vk::raii::Context _context;
    vk::raii::Instance _instance = nullptr;
    vk::raii::PhysicalDevice _physicalDevice = nullptr;
    vk::raii::DebugUtilsMessengerEXT _debugMessenger = nullptr;
    vk::raii::Device _device = nullptr;
    vk::raii::Queue _graphicsQueue = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;
    vk::raii::Queue _presentQueue = nullptr;

public:
    void run() {
        initVulkan();
        initWindow();
        mainLoop();
        cleanup();
    }
private:
    void initVulkan() {
        createInstance();
        // setupDebugMessenger();
        pickPhysicalDevice();
    }

    void initWindow() {
        glfwInit(); // Init glfw
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No API beceause OpenGL by default
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable resizing because more work

        constexpr int HEIGHT = 800;
        constexpr int WIDTH = 600;
        const std::string TITLE = "Vulkan";
        // Pass parameters by a WindowInfo struct to avoid errors like inverting width and height
        _window = glfwCreateWindow(WIDTH, HEIGHT, TITLE.c_str(), nullptr, nullptr);

    }

    void mainLoop() {
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void createInstance() {
        // Some elements are optional, replace by builder pattern
        constexpr vk::ApplicationInfo APP_INFO{
            .pApplicationName   = "Hello Triangle",
            .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
            .pEngineName        = "No Engine",
            .engineVersion      = VK_MAKE_VERSION( 1, 0, 0 ),
            .apiVersion         = vk::ApiVersion13
        };


        // Get the required layers
        std::vector<char const*> requiredLayers;
        if (enableValidationLayers) {
            requiredLayers.assign(validationLayers.begin(), validationLayers.end());
        }

        // Check if the required layers are supported by the Vulkan implementation.
        auto layerProperties = _context.enumerateInstanceLayerProperties();
        if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
            return std::ranges::none_of(layerProperties,
                                       [requiredLayer](auto const& layerProperty)
                                       { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
        }))
        {
            throw std::runtime_error("One or more required layers are not supported!");
        }

        for (auto layer: requiredLayers) {
            std::cout << layer << std::endl;
        }

        // Get the required instance extensions from GLFW.
        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        auto extensions = _context.enumerateInstanceExtensionProperties();
        std::cout << "available extensions:\n";

        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }

        // Check if the required GLFW extensions are supported by the Vulkan implementation.
        auto extensionProperties = _context.enumerateInstanceExtensionProperties();
        for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        {
            if (std::ranges::none_of(extensionProperties,
                                     [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
                                     { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
            {
                throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
            }
        }

        vk::InstanceCreateInfo createInfo{
            .pApplicationInfo = &APP_INFO,
            .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
            .ppEnabledLayerNames     = requiredLayers.data(),
            .enabledExtensionCount = glfwExtensionCount,
            .ppEnabledExtensionNames = glfwExtensions,
        };

        _instance = vk::raii::Instance(_context, createInfo);
    }

    void setupDebugMessenger()
    {
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags(
    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{.messageSeverity = severityFlags,
                                                                              .messageType     = messageTypeFlags,
                                                                              .pfnUserCallback = &debugCallback};
        _debugMessenger = _instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
    }

    std::vector<const char*> deviceExtensions = {
        vk::KHRSwapchainExtensionName
    };

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                          vk::DebugUtilsMessageTypeFlagsEXT type,
                                                          const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                                          void *)
    {
        if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
            std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
        return vk::False;
    }

    void pickPhysicalDevice() {
        std::vector<vk::raii::PhysicalDevice> devices = _instance.enumeratePhysicalDevices();
        const auto devIter = std::ranges::find_if(devices,
        [&](auto const & device) {
                auto queueFamilies = device.getQueueFamilyProperties();
                bool isSuitable = device.getProperties().apiVersion >= VK_API_VERSION_1_3;
                const auto qfpIter = std::ranges::find_if(queueFamilies,
                []( vk::QueueFamilyProperties const & qfp )
                        {
                            return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
                        } );
                isSuitable = isSuitable && ( qfpIter != queueFamilies.end() );
                auto extensions = device.enumerateDeviceExtensionProperties( );
                bool found = true;
                for (auto const & extension : deviceExtensions) {
                    auto extensionIter = std::ranges::find_if(extensions, [extension](auto const & ext) {return strcmp(ext.extensionName, extension) == 0;});
                    found = found &&  extensionIter != extensions.end();
                }
                isSuitable = isSuitable && found;
                if (isSuitable) {
                    _physicalDevice = device;
                }
                return isSuitable;
        });
        if (devIter == devices.end()) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    uint32_t findQueueFamilies(vk::raii::PhysicalDevice physicalDevice) {
        // find the index of the first queue family that supports graphics
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

        // get the first index into queueFamilyProperties which supports graphics
        auto graphicsQueueFamilyProperty =
          std::find_if( queueFamilyProperties.begin(),
                        queueFamilyProperties.end(),
                        []( vk::QueueFamilyProperties const & qfp ) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; } );

        return static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
    }

    void createLogicalDevice() {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = _physicalDevice.getQueueFamilyProperties();
        uint32_t graphicsIndex = findQueueFamilies(_physicalDevice);

        // determine a queueFamilyIndex that supports present
        // first check if the graphicsIndex is good enough
        auto presentIndex = _physicalDevice.getSurfaceSupportKHR(graphicsIndex, *surface )
                                           ? graphicsIndex : static_cast<uint32_t>(queueFamilyProperties.size());

        if ( presentIndex == queueFamilyProperties.size() )
        {
            // the graphicsIndex doesn't support present -> look for another family index that supports both
            // graphics and present
            for ( size_t i = 0; i < queueFamilyProperties.size(); i++ )
            {
                if ( ( queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics ) &&
                     _physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( i ), *surface ) )
                {
                    graphicsIndex = static_cast<uint32_t>( i );
                    presentIndex  = graphicsIndex;
                    break;
                }
            }
            if ( presentIndex == queueFamilyProperties.size() )
            {
                // there's nothing like a single family index that supports both graphics and present -> look for another
                // family index that supports present
                for ( size_t i = 0; i < queueFamilyProperties.size(); i++ )
                {
                    if ( _physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( i ), *surface ) )
                    {
                        presentIndex = static_cast<uint32_t>( i );
                        break;
                    }
                }
            }
        }
        if ( ( graphicsIndex == queueFamilyProperties.size() ) || ( presentIndex == queueFamilyProperties.size() ) )
        {
            throw std::runtime_error( "Could not find a queue for graphics or present -> terminating" );
        }



        float queuePriority = 1.0f; // Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution using floating point numbers between 0.0 and 1.0
        //The currently available drivers will only allow you to create a small number of queues for each queue family, and you don’t really need more than one
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo { .queueFamilyIndex = graphicsIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };
        vk::PhysicalDeviceFeatures deviceFeatures;



        // Here maybe use a builder to add features to pnext
        // Create a chain of feature structures
        vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
            {},                               // vk::PhysicalDeviceFeatures2 (empty for now)
            {.dynamicRendering = true },      // Enable dynamic rendering from Vulkan 1.3
            {.extendedDynamicState = true },   // Enable extended dynamic state from the extension
        };

        vk::DeviceCreateInfo deviceCreateInfo{
            .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &deviceQueueCreateInfo,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data()
        };

        _device = vk::raii::Device(_physicalDevice, deviceCreateInfo);
        _graphicsQueue = vk::raii::Queue( _device, graphicsIndex, 0 );
        _presentQueue = vk::raii::Queue( _device, presentIndex, 0 );
    }

    // In App
    void createSurface() {
        VkSurfaceKHR _surface;
        if (glfwCreateWindowSurface(*_instance, _window, nullptr, &_surface) != 0) {
            throw std::runtime_error("failed to create window surface!");
        }
        surface = vk::raii::SurfaceKHR(_instance, _surface);
    }
};

template<typename... Features>
struct Test {
    vk::StructureChain<Features...> features;
};

template<typename T>
void printType()
{
#if defined(__clang__) || defined(__GNUC__)
    std::cout << __PRETTY_FUNCTION__ << '\n';
#elif defined(_MSC_VER)
    std::cout << __FUNCSIG__ << '\n';
#endif
}

template<typename FirstFeature, typename... OtherFeatures>
void testStructureChains(const vk::StructureChain<FirstFeature, OtherFeatures...> &features) {
    const auto &first = features.template get<FirstFeature>();
    printType<FirstFeature>();

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &first,
        .queueCreateInfoCount = 1,
        // .pQueueCreateInfos = &deviceQueueCreateInfo,
        // .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        // .ppEnabledExtensionNames = deviceExtensions.data()
    };
}

int main() {
    std::array<const char *, 1> vLayers {
        "VK_LAYER_KHRONOS_validation",
    };


    const vk::ApplicationInfo vkInfo {
        .pApplicationName   = "Hello Triangle",
        .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
        .pEngineName        = "No Engine",
        .engineVersion      = VK_MAKE_VERSION( 1, 0, 0 ),
        .apiVersion         = vk::ApiVersion13
    };

    const dodo::core::App::AppInfo appInfo {
        .width =  800,
        .height = 600,
        .title = "TestApplication"
    };

    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                         vk::PhysicalDeviceVulkan13Features,
                         vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
        {},                               // vk::PhysicalDeviceFeatures2 (empty for now)
        {.dynamicRendering = true },      // Enable dynamic rendering from Vulkan 1.3
        {.extendedDynamicState = true },   // Enable extended dynamic state from the extension
    };

    const dodo::core::VulkanContext::VulkanDebug debugInfo {
        .debugEnabled = true,
        .validationLayers = vLayers,
    };

     const dodo::core::VulkanContext::VulkanContextInfo<vk::PhysicalDeviceFeatures2,
                         vk::PhysicalDeviceVulkan13Features,
                         vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> vkCtxInfo {
        .vkInfo = vkInfo,
        .featureChain = featureChain,
        .debugInfo = debugInfo
    };

    auto ctx = dodo::core::DodoContext::createDodoContext(vkCtxInfo);
    auto app = dodo::core::App::createApp(*ctx, appInfo);
    if (app) {
        std::cout << "Success" << std::endl;
    } else {
        std::cout << app.error() << std::endl;
        return EXIT_FAILURE;
    }
    while (app->isRunning()) {
        app->pollEvents();
    }
    // HelloTriangleApplication app;
    // try {
    //     app.run();
    // } catch (const std::exception& e) {
    //     std::cerr << e.what() << std::endl;
    //     return EXIT_FAILURE;
    // }
    return EXIT_SUCCESS;
}