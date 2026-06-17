//
// Created by nicol on 21/02/2026.
//

#include "dodo/core/VulkanContext.hpp"

namespace dodo::core {

VulkanContext::VulkanContext(VulkanContext &&other) noexcept:
_context(std::exchange(other._context, vk::raii::Context())),
_instance(std::exchange(other._instance, nullptr)),
_physicalDevice(std::exchange(other._physicalDevice, nullptr)),
_debugMessenger(std::exchange(other._debugMessenger, nullptr)),
_device(std::exchange(other._device, nullptr)),
_graphicsQueue(std::exchange(other._graphicsQueue, nullptr)),
_presentationQueue(std::exchange(other._presentationQueue, nullptr)),
_surface(std::exchange(other._surface, nullptr))
{
}

VulkanContext &VulkanContext::operator=(VulkanContext &&other) noexcept {
    if (this != &other) {
        _context = std::exchange(other._context, vk::raii::Context());
        _instance = std::exchange(other._instance, nullptr);
        _physicalDevice = std::exchange(other._physicalDevice, nullptr);
        _debugMessenger = std::exchange(other._debugMessenger, nullptr);
        _device = std::exchange(other._device, nullptr);
        _graphicsQueue = std::exchange(other._graphicsQueue, nullptr);
        _presentationQueue = std::exchange(other._presentationQueue, nullptr);
        _surface = std::exchange(other._surface, nullptr);
    }
    return *this;
}

auto VulkanContext::createInstance(const vk::ApplicationInfo &vkInfo, const VulkanDebug &debugInfo) -> std::expected<bool, std::string> {
    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (debugInfo.debugEnabled) {
        if (auto result = checkRequiredLayers(debugInfo.validationLayers); !result)
            return std::unexpected(result.error());
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    if (auto result = checkRequiredExtensions(std::span(extensions)); !result)
        return std::unexpected(result.error());

    const vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &vkInfo,
        .enabledLayerCount = static_cast<uint32_t>(debugInfo.validationLayers.size()),
        .ppEnabledLayerNames = debugInfo.validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };
    _instance = vk::raii::Instance(_context, createInfo);

    if (debugInfo.debugEnabled)
        setupDebugMessenger(debugInfo.debugCallback);
    return true;
}

auto VulkanContext::checkRequiredLayers(std::span<const char * const> validationLayers) const -> std::expected<bool, std::string> {
    if (!validationLayers.empty()) {
        auto layerProperties = _context.enumerateInstanceLayerProperties();
        if (std::ranges::any_of(validationLayers, [&layerProperties](auto const &requiredLayer) {
            return std::ranges::none_of(layerProperties, [requiredLayer](auto const &layerProperty) {
                return strcmp(layerProperty.layerName, requiredLayer) == 0;
            });
        })) {
            return std::unexpected("One or more required layers are not supported!");
        }
    }
    return true;
}

auto VulkanContext::checkRequiredExtensions(std::span<const char * const> glfwExtensions) const -> std::expected<bool, std::string> {
    auto extensionProperties = _context.enumerateInstanceExtensionProperties();

    std::cout << "available extensions:\n";
    for (const auto &[extensionName, specVersion] : extensionProperties) {
        std::cout << '\t' << extensionName << " " << specVersion << '\n';
    }

    for (auto glfwExtension : glfwExtensions) {
        if (std::ranges::none_of(extensionProperties,
                                 [glfwExtension](auto const& extensionProperty)
                                 { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; })) {
            return std::unexpected("Required GLFW extension not supported: " + std::string(glfwExtension));
        }
    }
    return true;
}

void VulkanContext::setupDebugMessenger(DebugCallback debugCallback) {
    // TODO: Move this out of function
    constexpr vk::DebugUtilsMessageSeverityFlagsEXT SEVERITY_FLAGS(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    constexpr vk::DebugUtilsMessageTypeFlagsEXT MESSAGE_TYPE_FLAGS(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );
    const vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
        .messageSeverity = SEVERITY_FLAGS,
        .messageType = MESSAGE_TYPE_FLAGS,
        .pfnUserCallback = debugCallback
    };
    _debugMessenger = _instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanContext::defaultDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                      vk::DebugUtilsMessageTypeFlagsEXT type,
                                                      const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                                      void *) {
    if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
    return vk::False;
}

auto VulkanContext::pickPhysicalDevice(const std::vector<const char *> &deviceExtensions) -> std::expected<bool, std::string> {
    auto devices = _instance.enumeratePhysicalDevices();
    const auto devIter = std::ranges::find_if(devices,
    [&](auto const & device) {
            auto queueFamilies = device.getQueueFamilyProperties();
            bool isSuitable = device.getProperties().apiVersion >= VK_API_VERSION_1_3;
            const auto qfpIter = std::ranges::find_if(queueFamilies,
            []( vk::QueueFamilyProperties const &qfp){
                return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
            });
            isSuitable = isSuitable && (qfpIter != queueFamilies.end());
            auto extensions = device.enumerateDeviceExtensionProperties();
            bool found = true;
            for (auto const &extension : deviceExtensions) {
                auto extensionIter = std::ranges::find_if(extensions, [extension](auto const & ext) {return strcmp(ext.extensionName, extension) == 0;});
                found = found && extensionIter != extensions.end();
            }
            isSuitable = isSuitable && found;
            if (isSuitable)
                _physicalDevice = device;
            return isSuitable;
    });
    if (devIter == devices.end())
        return std::unexpected("Failed to find a suitable GPU");
    return true;
}

bool VulkanContext::isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice) {
    const auto deviceProperties = physicalDevice.getProperties();
    const auto deviceFeatures = physicalDevice.getFeatures();

    return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && deviceFeatures.geometryShader;
}

auto VulkanContext::findGraphicQueueFamily(const vk::raii::PhysicalDevice &physicalDevice) const -> std::expected<uint32_t, std::string> {
   const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto graphicsQueueFamilyProperty =
      std::find_if( queueFamilyProperties.begin(),
                    queueFamilyProperties.end(),
                    []( vk::QueueFamilyProperties const & qfp ) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; } );
    if (graphicsQueueFamilyProperty == queueFamilyProperties.end())
        return std::unexpected("No graphics queue family found");
    return static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
}

auto VulkanContext::findPresentationQueueFamily(const vk::raii::PhysicalDevice &physicalDevice, uint32_t &graphicsIndex) const -> std::expected<uint32_t, std::string> {
    const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    const size_t queueFamilyPropertiesCount = queueFamilyProperties.size();

    auto presentationIndex = physicalDevice.getSurfaceSupportKHR(graphicsIndex, *_surface)
    ? graphicsIndex : static_cast<uint32_t>(queueFamilyProperties.size());

    if (presentationIndex == queueFamilyPropertiesCount) {
        for (size_t i = 0; i < queueFamilyPropertiesCount; i++) {
            if((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *_surface)) {
                graphicsIndex = static_cast<uint32_t>(i);
                presentationIndex = graphicsIndex;
                break;
            }
        }
        if (presentationIndex == queueFamilyPropertiesCount) {
            for (size_t i = 0; i < queueFamilyPropertiesCount; i++) {
                if (physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( i ), *_surface)) {
                    presentationIndex = static_cast<uint32_t>( i );
                    break;
                }
            }
        }
    }
    if (graphicsIndex == queueFamilyPropertiesCount)
        return std::unexpected("Could not find a queue family for graphics");
    if (presentationIndex == queueFamilyPropertiesCount)
        return std::unexpected("Could not find a queue family for presentation");
    return  presentationIndex;
}


auto VulkanContext::createSurface(const std::experimental::observer_ptr<GLFWwindow> &window) -> std::expected<bool, std::string> {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*_instance, window.get(), nullptr, &surface) != 0)
        return std::unexpected("failed to create window surface!");
    _surface = vk::raii::SurfaceKHR(_instance, surface);
    return true;
}

}  // namespace dodo::core
