#include "glfw_wrapper.hpp"

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include "fmt/base.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string_view>
#include <utility>

namespace glfw {

Terminate::~Terminate() {
    glfwTerminate();
}

auto init() -> Terminate {
    static auto done = false;
    // Can init just once
    assert(!done);
    if (GLFW_FALSE == glfwInit()) {
        fmt::println(stderr, "Failed to init GLFW");
        std::abort();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    done = true;
    return Terminate{};
}

auto Window::make(Terminate const &, int w, int h, std::string_view title) -> Window {
    auto *window = glfwCreateWindow(w, h, title.data(), nullptr, nullptr);
    if (window == nullptr) {
        fmt::println(stderr, "Cannot create window");
        std::abort();
    }
    return Window{window};
}

auto Window::make_context_current() -> void {
    glfwMakeContextCurrent(m_window);
}

auto Window::set_on_frame_buffer_resize_handler(std::unique_ptr<OnFrameBufferResizeHandler> handler)
    -> void {
    assert(handler != nullptr);
    m_on_resize_handler = std::move(handler);
    glfwSetWindowUserPointer(m_window, m_on_resize_handler.get());
    auto const cb = [](GLFWwindow *window, int w, int h) noexcept {  // NOLINT
        auto *ptr = glfwGetWindowUserPointer(window);
        if (ptr == nullptr) {
            return;
        }
        static_cast<OnFrameBufferResizeHandler *>(ptr)->on_resize(w, h);
    };
    glfwSetFramebufferSizeCallback(m_window, cb);
}

auto Window::remove_frame_buffer_resize_handler() -> void {
    m_on_resize_handler.reset();
    glfwSetWindowUserPointer(m_window, nullptr);
}

auto Window::swap_buffers() -> void {
    glfwSwapBuffers(m_window);
}

auto Window::should_close() -> bool {
    return glfwWindowShouldClose(m_window) != 0;
}

auto Window::set_should_close() -> void {
    glfwSetWindowShouldClose(m_window, 1);
}

auto Window::get_key(int key) -> int {
    return glfwGetKey(m_window, key);
}

auto Window::handle_input() -> void {
    for (auto const &cb : m_callbacks) {
        cb(*this);
    }
}

auto Window::add_callback(std::function<void(Window &)> cb) -> void {
    m_callbacks.push_back(std::move(cb));
}

auto loader_fn(const char *proc_name) noexcept -> void * {
    return reinterpret_cast<void *>(glfwGetProcAddress(proc_name));  // NOLINT
}
}  // namespace glfw
