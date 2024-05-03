#ifndef GL_VAO_HPP
#define GL_VAO_HPP

#include "glad/glad.h"
#include <utility>

namespace gl {
class Vao {
public:
    Vao(Vao const &) = delete;
    auto operator=(Vao const &) -> Vao & = delete;

    Vao(Vao &&other) noexcept
        : m_vao{std::exchange(other.m_vao, 0U)} {
    }

    auto operator=(Vao &&other) noexcept -> Vao & {
        m_vao = std::exchange(other.m_vao, 0U);
        return *this;
    }

    Vao() {
        glGenVertexArrays(1, &m_vao);
    }

    ~Vao() {
        if (m_vao != 0) {
            glDeleteVertexArrays(1, &m_vao);
        }
    }

    auto bind() const -> void {
        glBindVertexArray(m_vao);
    }

    static auto unbind() -> void {
        glBindVertexArray(0);
    }

private:
    GLuint m_vao{};
};
}  // namespace gl

#endif
