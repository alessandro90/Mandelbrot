#ifndef GL_VAO_HPP
#define GL_VAO_HPP

#include "buffer.hpp"
#include "glad/glad.h"
#include <utility>
#include <vector>

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

    template <typename T, std::size_t N>
    auto add_buffer(BufferType type, std::span<T, N> data) -> GLuint {
        bind();
        m_buffers.push_back(StaticDrawBuffer::make(type, data));
        unbind();
        return m_buffers.back().id();
    }

    auto vertex_attrib_ptr(GLuint index,
                           GLint size,
                           GLenum type,
                           GLboolean normalized,
                           GLsizei stride,
                           void const *pointer) const -> void {
        bind();
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        unbind();
    }

    auto enable(GLuint index) const -> void {
        bind();
        glEnableVertexAttribArray(index);
        unbind();
    }

    auto disable(GLuint index) const -> void {
        bind();
        glDisableVertexAttribArray(index);
        unbind();
    }

private:
    GLuint m_vao{};
    std::vector<StaticDrawBuffer> m_buffers;
};
}  // namespace gl

#endif
