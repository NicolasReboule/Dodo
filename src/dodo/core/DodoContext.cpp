//
// Created by nicol on 21/02/2026.
//

#include "dodo/core/DodoContext.hpp"

namespace dodo::core {

template <typename... Features>
auto DodoContext::createDodoContext(const VulkanContext::VulkanContextInfo<Features...> &ctxInfo)
    -> std::expected<DodoContext, std::string> {
    auto glfwContext = GLFWContext::createContext();
    if (!glfwContext)
        return std::unexpected("Failed to create GLFW context: " + glfwContext.error());
    auto vulkanContext = VulkanContext::createContext(ctxInfo);
    if (!vulkanContext)
        return std::unexpected("Failed to create Vulkan context: " + vulkanContext.error());
    DodoContext dodoContext;
    dodoContext._glfwContext = std::move(*glfwContext);
    dodoContext._vulkanContext = std::move(*vulkanContext);
    return dodoContext;
}

DodoContext::DodoContext(DodoContext &&other) noexcept:
_glfwContext(std::move(other._glfwContext)),
_vulkanContext(std::move(other._vulkanContext))
{
}

DodoContext& DodoContext::operator=(DodoContext &&other) noexcept {
    if (this != &other) {
        _glfwContext = std::move(other._glfwContext);
        _vulkanContext = std::move(other._vulkanContext);
    }
    return *this;
}

}  // namespace dodo::core