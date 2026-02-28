//
// Created by nicol on 21/02/2026.
//

#include "dodo/core/DodoContext.hpp"

namespace dodo::core {

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