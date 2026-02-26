//
// Created by nicol on 21/02/2026.
//

#include "dodo/core/GLFWContext.hpp"

#include <iostream>

namespace dodo::core {

auto GLFWContext::createContext() -> std::expected<GLFWContext, std::string> {
    if (_initialized) return std::unexpected("GLFW is already initialized");
    if (const char* errorBuf; !glfwInit()) {
        glfwGetError(&errorBuf);
        return std::unexpected(errorBuf);
    }
    _initialized = true;
    return GLFWContext();
}

GLFWContext::GLFWContext(GLFWContext &&other) noexcept
    : _ownsContext(std::exchange(other._ownsContext, false)) {}

GLFWContext &GLFWContext::operator=(GLFWContext &&other) noexcept {
    if (this != &other) _ownsContext = std::exchange(other._ownsContext, false);
    return *this;
}

GLFWContext::~GLFWContext() {
    if (_ownsContext) {
        glfwTerminate();
        _initialized = false;
    }
}

}  // namespace dodo::core