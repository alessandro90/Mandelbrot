#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include "fmt/base.h"
#include "fmt/core.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <utility>

#include "shader.hpp"

namespace {
constexpr auto g_width = 800;
constexpr auto g_height = 600;

auto process_input(GLFWwindow *window) noexcept -> void {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}
}  // namespace

auto main() -> int {
    if (GLFW_FALSE == glfwInit()) {
        fmt::println(stderr, "Failed to init GLFW");
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto *window = glfwCreateWindow(g_width, g_height, "Learn OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        fmt::println(stderr, "Cannot create window");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    if (gladLoadGLLoader([](const char *proc_name) noexcept -> void * {
            return reinterpret_cast<void *>(glfwGetProcAddress(proc_name));  // NOLINT
        })
        == 0) {
        fmt::println(stderr, "Failed to initialize GLAD");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glViewport(0, 0, g_width, g_height);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int w, int h) noexcept {  // NOLINT
        glViewport(0, 0, w, h);
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

    auto vao = GLuint{};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    auto vbo = GLuint{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(vertices[0]),
                 vertices.data(),
                 GL_STATIC_DRAW);

    auto ebo = GLuint{};
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indeces.size() * sizeof(indeces[0]),
                 indeces.data(),
                 GL_STATIC_DRAW);


    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(float),          // NOLINT
                          static_cast<GLvoid *>(0));  // NOLINT
    glEnableVertexAttribArray(0);

    // FIXME: We assume shaders are in the src directory (sibling of build)
    // and we assume the working directory is in fact 'build'
    auto maybe_shader = Shader::make("../src/shader.vert", "../src/shader.frag");
    if (!maybe_shader.has_value()) {
        return EXIT_FAILURE;
    }
    auto const shader = std::move(maybe_shader).value();

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);  // NOLINT
    glClear(GL_COLOR_BUFFER_BIT);
    while (glfwWindowShouldClose(window) == 0) {
        process_input(window);

        shader.use();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, static_cast<GLvoid *>(0));  // NOLINT
        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glfwTerminate();
}
