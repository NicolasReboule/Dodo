//
// Created by nicol on 21/02/2026.
//

#ifndef DODO_GLFWCONTEXT_HPP
#define DODO_GLFWCONTEXT_HPP

#include "GLFW/glfw3.h"
#include <string>
#include <expected>
#include <format>

namespace dodo::core {

class DodoContext;

class GLFWContext {
   private:
    static inline bool _initialized = false;
    bool _ownsContext = false;
   public:
    ~GLFWContext();

    GLFWContext(const GLFWContext &other) = delete;

    GLFWContext &operator=(const GLFWContext &other) = delete;

    GLFWContext(GLFWContext &&other) noexcept;

    GLFWContext &operator=(GLFWContext &&other) noexcept;

   private:
    GLFWContext() = default;

    static auto createContext() -> std::expected<GLFWContext, std::string>;

    friend class DodoContext;
};

}
#endif  // DODO_GLFWCONTEXT_HPP
