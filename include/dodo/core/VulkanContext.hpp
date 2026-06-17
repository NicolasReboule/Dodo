//
// Created by nicol on 21/02/2026.
//

#ifndef DODO_VULKANCONTEXT_HPP
#define DODO_VULKANCONTEXT_HPP

#include <expected>
#include <functional>
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include <experimental/memory>
#include <vulkan/vulkan_raii.hpp>

#include "GLFW/glfw3.h"

namespace dodo::core {

class DodoContext;

class VulkanContext {
private:
    vk::raii::Context _context;
    vk::raii::Instance _instance = nullptr;
    vk::raii::PhysicalDevice _physicalDevice = nullptr;
    vk::raii::DebugUtilsMessengerEXT _debugMessenger = nullptr;
    vk::raii::Device _device = nullptr;
    vk::raii::Queue _graphicsQueue = nullptr;
    vk::raii::Queue _presentationQueue = nullptr;
    vk::raii::SurfaceKHR _surface = nullptr; // this must be destroyed before _instance
public:
    using DebugCallback = vk::PFN_DebugUtilsMessengerCallbackEXT;

    struct VulkanDebug {
        bool debugEnabled = false;
        std::span<const char *const> validationLayers;
        DebugCallback debugCallback = defaultDebugCallback;
    };

    template<typename... Features>
    struct VulkanContextInfo {
        vk::ApplicationInfo vkInfo;
        vk::StructureChain<Features...> featureChain = vk::StructureChain<vk::PhysicalDeviceFeatures2>{};
        std::vector<const char *> deviceExtensions;
        VulkanDebug debugInfo = {};
    };

    ~VulkanContext() = default;

    VulkanContext(const VulkanContext &other) = delete;

    VulkanContext &operator=(const VulkanContext &other) = delete;

    VulkanContext(VulkanContext &&other) noexcept;

    VulkanContext &operator=(VulkanContext &&other) noexcept;

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL defaultDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                      vk::DebugUtilsMessageTypeFlagsEXT type,
                                                      const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                      void *);

    template<typename... Features>
    static auto createContext(const VulkanContextInfo<Features...> &ctxInfo, const std::experimental::observer_ptr<GLFWwindow> &window) -> std::expected<VulkanContext, std::string>;

private:
    VulkanContext() = default;

    auto createInstance(const vk::ApplicationInfo &vkInfo, const VulkanDebug &debugInfo) -> std::expected<bool, std::string>;

    auto checkRequiredLayers(std::span<const char * const> validationLayers) const -> std::expected<bool, std::string>;

    auto checkRequiredExtensions(std::span<const char * const> glfwExtensions) const -> std::expected<bool, std::string>;

    void setupDebugMessenger(DebugCallback debugCallback);

    auto pickPhysicalDevice(const std::vector<const char *> &deviceExtensions) -> std::expected<bool, std::string>;

    static bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice);

    template<typename FirstFeature, typename... OtherFeatures>
    static FirstFeature &getFirstFeature(vk::StructureChain<FirstFeature, OtherFeatures...> featureChain);

    auto findGraphicQueueFamily(const vk::raii::PhysicalDevice &physicalDevice) const -> std::expected<uint32_t, std::string>;

    auto findPresentationQueueFamily(const vk::raii::PhysicalDevice &physicalDevice, uint32_t &graphicsIndex) const -> std::expected<uint32_t, std::string>;

    template<typename... Features>
    auto createLogicalDevice(const VulkanContextInfo<Features ...> &ctxInfo) -> std::expected<bool, std::string>;

    auto createSurface(const std::experimental::observer_ptr<GLFWwindow> &window) -> std::expected<bool, std::string>;

    friend class DodoContext;
};

template<typename... Features>
auto VulkanContext::createContext(const VulkanContextInfo<Features...> &ctxInfo, const std::experimental::observer_ptr<GLFWwindow> &window)
-> std::expected<VulkanContext, std::string> {
    VulkanContext vulkanContext;
    if (auto result = vulkanContext.createInstance(ctxInfo.vkInfo, ctxInfo.debugInfo); !result)
        return std::unexpected(result.error());
    if (auto result = vulkanContext.createSurface(window); !result)
        return std::unexpected(result.error());
    if (auto result = vulkanContext.pickPhysicalDevice(ctxInfo.deviceExtensions); !result)
        return std::unexpected(result.error());
    if (auto result = vulkanContext.createLogicalDevice(ctxInfo); !result)
        return std::unexpected(result.error());
    return vulkanContext;
}

template<typename FirstFeature, typename... OtherFeatures>
FirstFeature &VulkanContext::getFirstFeature(vk::StructureChain<FirstFeature, OtherFeatures...> featureChain) {
    return featureChain.template get<FirstFeature>();
}

template<typename... Features>
auto VulkanContext::createLogicalDevice(const VulkanContextInfo<Features ...> &ctxInfo) -> std::expected<bool, std::string> {
    auto graphicsIndex = findGraphicQueueFamily(_physicalDevice);
    if (!graphicsIndex.has_value())
        return std::unexpected(graphicsIndex.error());

    auto presentationIndex = findPresentationQueueFamily(_physicalDevice, *graphicsIndex);
    if (!presentationIndex.has_value())
        return std::unexpected(presentationIndex.error());

    constexpr float queuePriority = 1.0F;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo { .queueFamilyIndex = *graphicsIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };

    const vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &getFirstFeature(ctxInfo.featureChain),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(ctxInfo.deviceExtensions.size()),
        .ppEnabledExtensionNames = ctxInfo.deviceExtensions.data()
    };

    _device = vk::raii::Device(_physicalDevice, deviceCreateInfo);
    _graphicsQueue = vk::raii::Queue(_device, *graphicsIndex, 0);
    _presentationQueue = vk::raii::Queue(_device, *presentationIndex, 0);
    return true;
}

}

#endif  // DODO_VULKANCONTEXT_HPP
