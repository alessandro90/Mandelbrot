#include "app.hpp"

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include "fmt/base.h"
#include "fmt/core.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <memory>
#include <span>
#include <string_view>
#include <utility>

#include "buffer.hpp"
#include "glfw_wrapper.hpp"
#include "program.hpp"
#include "vao.hpp"

using namespace std::string_view_literals;

namespace {

constexpr auto g_width = 800;
constexpr auto g_height = 600;

auto draw() -> void {
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, static_cast<GLvoid *>(0));  // NOLINT
}

auto fill_bg() -> void {
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);  // NOLINT
    glClear(GL_COLOR_BUFFER_BIT);
}

auto set_viewport(int w, int h) {
    glViewport(0, 0, w, h);
}

class OnFrameBuffferResize: public glfw::Window::OnFrameBufferResizeHandler {
public:
    explicit OnFrameBuffferResize(gl::Program const &prog)
        : m_prog{prog} {
    }

    // NOLINTNEXTLINE
    auto on_resize(int w, int h) -> void override {
        set_viewport(w, h);
        m_prog.get().set_uniform("view_port"sv,
                                 std::pair{static_cast<float>(w), static_cast<float>(h)});
    }

private:
    std::reference_wrapper<gl::Program const> m_prog;
};

}  // namespace

auto App::run() -> void {
    auto const resource_cleaner = glfw::init();

    auto window = glfw::Window::make(resource_cleaner, g_width, g_height, "Mandelbrot"sv);
    window.make_context_current();

    if (gladLoadGLLoader(glfw::loader_fn) == 0) {
        fmt::println(stderr, "Failed to initialize GLAD");
        std::abort();
    }

    set_viewport(g_width, g_height);


    window.add_callback([](glfw::Window &w) {
        if (w.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            w.set_should_close();
        }
    });

    static constexpr auto vertices = std::array{
        // clang-format off
         1.0F,  1.0F, 0.0F, // NOLINT top right
         1.0F, -1.0F, 0.0F, // NOLINT bottom right
        -1.0F, -1.0F, 0.0F, // NOLINT bottom left
        -1.0F,  1.0F, 0.0F  // NOLINT top left
        // clang-format on
    };

    static constexpr auto indeces = std::array<GLuint, 6U>{
        // clang-format off
        0, 1, 3, // first triangle
        1, 2, 3  // second triagle
        // clang-format on
    };

    auto vao = gl::Vao{};
    // VBO
    vao.add_buffer(gl::BufferType::ARRAY, std::span{vertices});
    // EBO
    vao.add_buffer(gl::BufferType::ELEMENT_ARRAY, std::span{indeces});
    vao.vertex_attrib_ptr(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(float),
                          static_cast<GLvoid *>(0));  // NOLINT
    vao.enable(0);
    vao.bind();

    // FIXME: We assume shaders are in the src directory (sibling of build)
    // and we assume the working directory is in fact 'build'
    auto const program = []() {
        auto p = gl::Program::create_and_link({"../src/shader.vert"sv, "../src/shader.frag"sv});
        if (!p.has_value()) {
            fmt::println(stderr, "Cannot create program.");
            std::abort();
        }
        return std::move(p).value();
    }();
    program.use();
    window.set_on_frame_buffer_resize_handler(std::make_unique<OnFrameBuffferResize>(program));

    fill_bg();
    while (!window.should_close()) {
        window.handle_input();
        draw();
        glfwPollEvents();
        window.swap_buffers();
    }
}
