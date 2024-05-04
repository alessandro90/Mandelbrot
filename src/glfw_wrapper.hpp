#ifndef GLFW_WRAP_HPP
#define GLFW_WRAP_HPP

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

namespace glfw {
class Terminate {
    friend auto init() -> Terminate;

public:
    Terminate(Terminate const &) = delete;
    auto operator=(Terminate const &) -> Terminate & = delete;

    Terminate(Terminate &&other) noexcept {
        other.m_moved = true;
    }

    auto operator=(Terminate &&other) noexcept -> Terminate & {
        other.m_moved = true;
        return *this;
    }

    ~Terminate();

private:
    Terminate() = default;

    bool m_moved{false};
};

[[nodiscard]] auto init() -> Terminate;
auto loader_fn(const char *proc_name) noexcept -> void *;

class Window {
public:
    class OnFrameBufferResizeHandler {
    public:
        virtual ~OnFrameBufferResizeHandler() = default;
        virtual auto on_resize(int w, int h) -> void = 0;
    };

    [[nodiscard]] static auto make(Terminate const &,
                                   int w,
                                   int h,
                                   std::string_view title) -> Window;

    auto make_context_current() -> void;

    auto set_on_frame_buffer_resize_handler(std::unique_ptr<OnFrameBufferResizeHandler> handler)
        -> void;
    auto remove_frame_buffer_resize_handler() -> void;

    auto swap_buffers() -> void;

    auto handle_input() -> void;

    [[nodiscard]] auto should_close() -> bool;
    auto set_should_close() -> void;

    [[nodiscard]] auto get_key(int key) -> int;

    auto add_callback(std::function<void(Window &)> cb) -> void;

private:
    explicit Window(GLFWwindow *window)
        : m_window{window} {
    }

    GLFWwindow *m_window;
    std::unique_ptr<OnFrameBufferResizeHandler> m_on_resize_handler;
    std::vector<std::function<void(Window &)>> m_callbacks;
};
}  // namespace glfw

#endif
