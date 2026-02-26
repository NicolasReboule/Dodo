//
// Created by nicol on 21/02/2026.
//

#ifndef DODO_DODOCONTEXT_HPP
#define DODO_DODOCONTEXT_HPP

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

}

#endif  // DODO_DODOCONTEXT_HPP
