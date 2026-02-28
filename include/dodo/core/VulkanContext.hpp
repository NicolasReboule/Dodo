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
public:
    using DebugCallback = PFN_vkDebugUtilsMessengerCallbackEXT;

    struct VulkanDebug {
        bool debugEnabled = false;
        std::span<const char *const> validationLayers;
        DebugCallback debugCallback = defaultDebugCallback;
    };

    template<typename... Features>
    struct VulkanContextInfo {
        vk::ApplicationInfo vkInfo;
        vk::StructureChain<Features...> featureChain = vk::StructureChain<vk::PhysicalDeviceFeatures2>{};
        VulkanDebug debugInfo = {};
    };

    ~VulkanContext() = default;

    VulkanContext(const VulkanContext &other) = delete;

    VulkanContext &operator=(const VulkanContext &other) = delete;

    VulkanContext(VulkanContext &&other) noexcept;

    VulkanContext &operator=(VulkanContext &&other) noexcept;

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL defaultDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *);

private:
    VulkanContext() = default;

    template<typename... Features>
    static auto createContext(const VulkanContextInfo<Features...> &ctxInfo) -> std::expected<VulkanContext, std::string>;

    static auto checkRequiredLayers(const VulkanContext &vulkanContext, std::span<const char * const> validationLayers) -> std::expected<bool, std::string>;

    static auto checkRequiredExtensions(const VulkanContext &vulkanContext, std::span<const char * const> glfwExtensions) -> std::expected<bool, std::string>;

    static void setupDebugMessenger(VulkanContext &vulkanContext, DebugCallback debugCallback);

    static auto pickPhysicalDevice(VulkanContext &vulkanContext) -> std::expected<bool, std::string>;

    static bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice);

    template<typename FirstFeature, typename... OtherFeatures>
    static FirstFeature &getFirstFeature(vk::StructureChain<FirstFeature, OtherFeatures...> featureChain);

    friend class DodoContext;
};

template<typename... Features>
auto VulkanContext::createContext(const VulkanContextInfo<Features...> &ctxInfo)
-> std::expected<VulkanContext, std::string> {
    VulkanContext vulkanContext;
    VulkanDebug debugInfo = ctxInfo.debugInfo;
    if (debugInfo.debugEnabled) {
        if (auto result = checkRequiredLayers(vulkanContext, debugInfo.validationLayers); !result)
            return std::unexpected(result.error());
        // setupDebugMessenger(vulkanContext, debugInfo.debugCallback);
    }

    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (auto result = checkRequiredExtensions(vulkanContext, std::span(glfwExtensions, glfwExtensionCount)); !result)
        return std::unexpected(result.error());

    const vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &ctxInfo.vkInfo,
        .enabledLayerCount = static_cast<uint32_t>(debugInfo.validationLayers.size()),
        .ppEnabledLayerNames = debugInfo.validationLayers.data(),
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions,
    };

    vulkanContext._instance = vk::raii::Instance(vulkanContext._context, createInfo);
    return vulkanContext;
}

}

#endif  // DODO_VULKANCONTEXT_HPP
