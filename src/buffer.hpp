#ifndef GL_BUFFER_HPP
#define GL_BUFFER_HPP

#include "glad/glad.h"
#include <cstddef>
#include <span>
#include <utility>

namespace gl {
enum class BufferType : GLint {
    ARRAY = GL_ARRAY_BUFFER,
    ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER,
};

class StaticDrawBuffer {
public:
    template <typename T, std::size_t N>
    [[nodiscard]] static auto make(BufferType type, std::span<T, N> data) -> StaticDrawBuffer {
        auto buf = GLuint{};
        glGenBuffers(1, &buf);
        auto obj = StaticDrawBuffer{buf, type};
        obj.bind();
        obj.set_data(data);
        return obj;
    }

    StaticDrawBuffer(StaticDrawBuffer const &) = delete;
    auto operator=(StaticDrawBuffer const &) -> StaticDrawBuffer & = delete;

    StaticDrawBuffer(StaticDrawBuffer &&other) noexcept
        : m_buf{std::exchange(other.m_buf, 0U)}
        , m_type{other.m_type} {
    }

    auto operator=(StaticDrawBuffer &&other) noexcept -> StaticDrawBuffer & {
        m_buf = std::exchange(other.m_buf, 0U);
        m_type = other.m_type;
        return *this;
    }

    ~StaticDrawBuffer() {
        if (m_buf != 0) {
            glDeleteBuffers(1, &m_buf);
        }
    }

    auto bind() const -> void {
        glBindBuffer(std::to_underlying(m_type), m_buf);
    }

    template <typename T, std::size_t N>
    auto set_data(std::span<T, N> data) const -> void {
        bind();
        glBufferData(std::to_underlying(m_type),
                     data.size() * sizeof(data[0]),
                     data.data(),
                     GL_STATIC_DRAW);
    }

private:
    explicit StaticDrawBuffer(GLuint buf, BufferType type)
        : m_buf{buf}
        , m_type{type} {
    }

    GLuint m_buf;
    BufferType m_type;
};
}  // namespace gl

#endif
