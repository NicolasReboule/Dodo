//
// Created by nicol on 21/02/2026.
//

#include "dodo/core/VulkanContext.hpp"

namespace dodo::core {


VulkanContext::VulkanContext(VulkanContext &&other) noexcept:
_context(std::exchange(other._context, vk::raii::Context())),
_instance(std::exchange(other._instance, nullptr)),
_physicalDevice(std::exchange(other._physicalDevice, nullptr))
{
}

VulkanContext &VulkanContext::operator=(VulkanContext &&other) noexcept {
    if (this != &other) {
        _context = std::exchange(other._context, vk::raii::Context());
        _instance = std::exchange(other._instance, nullptr);
        _physicalDevice = std::exchange(other._physicalDevice, nullptr);
    }
    return *this;
}


auto VulkanContext::checkRequiredLayers(const VulkanContext &vulkanContext, std::span<const char * const> validationLayers) -> std::expected<bool, std::string> {
    if (!validationLayers.empty()) {
        auto layerProperties = vulkanContext._context.enumerateInstanceLayerProperties();
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

auto VulkanContext::checkRequiredExtensions(const VulkanContext &vulkanContext, std::span<const char * const> glfwExtensions) -> std::expected<bool, std::string> {
    auto extensionProperties = vulkanContext._context.enumerateInstanceExtensionProperties();

    std::cout << "available extensions:\n";
    for (const auto &[extensionName, specVersion] : extensionProperties) {
        std::cout << '\t' << extensionName << " " << specVersion << '\n';
    }

    for (auto glfwExtension : glfwExtensions)
    {
        if (std::ranges::none_of(extensionProperties,
                                 [glfwExtension](auto const& extensionProperty)
                                 { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
        {
            return std::unexpected("Required GLFW extension not supported: " + std::string(glfwExtension));
        }
    }
    return true;
}

void VulkanContext::setupDebugMessenger(VulkanContext &vulkanContext, DebugCallback debugCallback) {
    constexpr vk::DebugUtilsMessageSeverityFlagsEXT SEVERITY_FLAGS(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    constexpr vk::DebugUtilsMessageTypeFlagsEXT MESSAGE_TYPE_FLAGS(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    const vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{.messageSeverity = SEVERITY_FLAGS,
                                                                          .messageType = MESSAGE_TYPE_FLAGS,
                                                                          .pfnUserCallback = debugCallback};
    vulkanContext._debugMessenger = vulkanContext._instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanContext::defaultDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *)
{
    if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT || severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cerr << "validation layer: type " << type << " msg: " << pCallbackData->pMessage << std::endl;
    return vk::False;
}

auto VulkanContext::pickPhysicalDevice(VulkanContext &vulkanContext) -> std::expected<bool, std::string> {
    // auto devices = vulkanContext._instance.enumeratePhysicalDevices();
    // const auto devIter = std::ranges::find_if(devices,
    // [&](auto const & device) {
    //         auto queueFamilies = device.getQueueFamilyProperties();
    //         bool isSuitable = device.getProperties().apiVersion >= VK_API_VERSION_1_3;
    //         const auto qfpIter = std::ranges::find_if(queueFamilies,
    //         []( vk::QueueFamilyProperties const & qfp )
    //                 {
    //                     return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
    //                 } );
    //         isSuitable = isSuitable && ( qfpIter != queueFamilies.end() );
    //         auto extensions = device.enumerateDeviceExtensionProperties( );
    //         bool found = true;
    //         for (auto const & extension : deviceExtensions) {
    //             auto extensionIter = std::ranges::find_if(extensions, [extension](auto const & ext) {return strcmp(ext.extensionName, extension) == 0;});
    //             found = found && extensionIter != extensions.end();
    //         }
    //         isSuitable = isSuitable && found;
    //         if (isSuitable)
    //             vulkanContext._physicalDevice = device;
    //         return isSuitable;
    // });
    // if (devIter == devices.end())
    //     return std::unexpected("Failed to find a suitable GPU");
    return true;
}

bool VulkanContext::isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice) {
    const auto deviceProperties = physicalDevice.getProperties();
    const auto deviceFeatures = physicalDevice.getFeatures();

    return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && deviceFeatures.geometryShader;
}

template<typename FirstFeature, typename... OtherFeatures>
FirstFeature &VulkanContext::getFirstFeature(vk::StructureChain<FirstFeature, OtherFeatures...> featureChain) {
    return featureChain.template get<FirstFeature>();
}

}  // namespace dodo::core
