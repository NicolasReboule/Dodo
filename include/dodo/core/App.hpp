//
// Created by nicol on 19/02/2026.
//

#ifndef DODO_CORE_HPP
#define DODO_CORE_HPP

#include <expected>
#include <experimental/memory>
#include <format>
#include <string>
#include <utility>
#include <vulkan/vulkan_raii.hpp>

#include "GLFW/glfw3.h"
#include "GLFWContext.hpp"

namespace dodo::core {

class App {
   private:
    GLFWwindow *_window = nullptr;

    // I believe I should add contexts here and remove dodoContext entirely
   public:
    struct AppInfo {
        size_t width;
        size_t height;
        std::string title;
        GLFWmonitor *monitor = nullptr;
    };

    ~App();

    App(const App &other) = delete;

    App &operator=(const App &other) = delete;

    App(App && other) noexcept;

    App &operator=(App && other) noexcept;

    static auto createApp(const GLFWContext &ctx, const AppInfo &appInfo) -> std::expected<App, std::string>;

    bool isRunning() const { return !glfwWindowShouldClose(_window); };

    void pollEvents() const { glfwPollEvents(); };

    std::experimental::observer_ptr<GLFWwindow> getWindow() const { return std::experimental::make_observer(_window); };
   private:
    App() = default;

    auto createGLFWWindow(const AppInfo &appInfo) -> std::expected<bool, std::string>;
};

};

#endif  // DODO_CORE_HPP
