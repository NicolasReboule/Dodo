//
// Created by nicol on 19/02/2026.
//

#include "dodo/core/App.hpp"

namespace dodo::core {

auto App::createApp(const DodoContext &ctx, const AppInfo &appInfo) -> std::expected<App, std::string> {
    App app;
    if (auto result = app.createGLFWWindow(appInfo); !result)
        return std::unexpected(result.error());
    return app;
}

auto App::createGLFWWindow(const AppInfo &appInfo) -> std::expected<bool, std::string> {
    const char *errorBuf;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(appInfo.width, appInfo.height, appInfo.title.c_str(), appInfo.monitor, nullptr);
    if (!_window) {
        glfwGetError(&errorBuf);
        return std::unexpected(std::format("Failed to create GLFW Window: {}", errorBuf));
    }
    return true;
}

App::App(App&& other) noexcept:
_window(std::exchange(other._window, nullptr))
{

}

App &App::operator=(App&& other) noexcept {
    _window = std::exchange(other._window, nullptr);
    return *this;
}

App::~App() {
    if (_window)
        glfwDestroyWindow(_window);
}

}  // namespace dodo::core
