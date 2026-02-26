//
// Created by nicol on 19/02/2026.
//

#ifndef DODO_CORE_HPP
#define DODO_CORE_HPP

#include <expected>
#include <format>
#include <string>

#include "DodoContext.hpp"
#include "GLFW/glfw3.h"

namespace dodo::core {

class App {
   private:
    GLFWwindow *_window = nullptr;
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

    static auto createApp(const DodoContext &ctx, const AppInfo &appInfo) -> std::expected<App, std::string>;

    bool isRunning() const { return !glfwWindowShouldClose(_window); };

    void pollEvents() const { glfwPollEvents(); };
   private:
    App() = default;

    auto createGLFWWindow(const AppInfo &appInfo) -> std::expected<bool, std::string>;
};

};

#endif  // DODO_CORE_HPP
