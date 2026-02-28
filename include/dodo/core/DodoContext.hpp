//
// Created by nicol on 21/02/2026.
//

#ifndef DODO_DODOCONTEXT_HPP
#define DODO_DODOCONTEXT_HPP

#include <utility>

#include "GLFWContext.hpp"
#include "VulkanContext.hpp"

namespace dodo::core {

class DodoContext {
private:
    GLFWContext _glfwContext;
    VulkanContext _vulkanContext;

    DodoContext() = default;
public:
    template<typename... Features>
    static auto createDodoContext(const VulkanContext::VulkanContextInfo<Features...> &ctxInfo)
     -> std::expected<DodoContext, std::string>;

    ~DodoContext() = default;

    DodoContext(const DodoContext &other) = delete;

    DodoContext &operator=(const DodoContext &other) = delete;

    DodoContext(DodoContext &&other) noexcept;

    DodoContext &operator=(DodoContext &&other) noexcept;
};


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

}

#endif  // DODO_DODOCONTEXT_HPP
