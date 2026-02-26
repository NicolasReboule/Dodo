//
// Created by nicol on 29/12/2024.
//

#include "dodo/core/App.hpp"

#include <gtest/gtest.h>

TEST(GeneralTest, TouchAllClasses) {
    const vk::ApplicationInfo vkInfo {
        .pApplicationName   = "Test",
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

    vk::StructureChain<vk::PhysicalDeviceFeatures2> featureChain = {
        {},
    };

    constexpr dodo::core::VulkanContext::VulkanDebug debugInfo {
        .debugEnabled = true,
    };

    const dodo::core::VulkanContext::VulkanContextInfo<vk::PhysicalDeviceFeatures2> vkCtxInfo {
        .vkInfo = vkInfo,
        .featureChain = featureChain,
        .debugInfo = debugInfo
    };

    auto ctx = dodo::core::DodoContext::createDodoContext(vkCtxInfo);
    if (!ctx.has_value())
        return;
    auto app = dodo::core::App::createApp(*ctx, appInfo);
}
