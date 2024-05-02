#ifndef SHADER_HPP
#define SHADER_HPP

#include "glad/glad.h"

#include "fmt/base.h"
#include <array>
#include <cstdio>
#include <fstream>
#include <optional>
#include <string_view>
#include <utility>

[[nodiscard]] inline auto read_file(std::string_view path) -> std::optional<std::string> {
    auto file = std::ifstream{path.data(), std::ios::binary};
    if (!file.is_open()) {
        return std::nullopt;
    }
    return std::string(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});
}

[[nodiscard]] inline auto check_shader_compile_error(GLuint shader,
                                                     std::string_view msg) noexcept -> bool {
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        auto info_log = std::array<char, 512U>{};  // NOLINT
        glGetShaderInfoLog(shader, info_log.size(), nullptr, info_log.data());
        fmt::println(stderr, "{}: {}", msg, info_log.data());
        return false;
    }
    return true;
}

[[nodiscard]] inline auto create_shader(const char *src, int type) -> std::optional<GLuint> {
    auto const shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    if (!check_shader_compile_error(shader, "Shader error")) {
        return std::nullopt;
    }
    return shader;
}

class Shader {
public:
    [[nodiscard]] auto id() const noexcept -> GLuint {
        return m_id.value();
    }

    void use() const {
        glUseProgram(m_id.value());
    }

    void set_bool(std::string_view name, bool value) const {
        glUniform1i(glGetUniformLocation(m_id.value(), name.data()), static_cast<GLint>(value));
    }

    void set_int(std::string_view name, int value) const {
        glUniform1i(glGetUniformLocation(m_id.value(), name.data()), value);
    }

    void set_float(std::string_view name, float value) const {
        glUniform1f(glGetUniformLocation(m_id.value(), name.data()), value);
    }

    [[nodiscard]] static auto make(std::string_view vertex_path,  // NOLINT
                                   std::string_view fragment_path) -> std::optional<Shader> {
        auto const vertex_src = read_file(vertex_path);
        auto const fragment_src = read_file(fragment_path);
        if (!vertex_src.has_value() || !fragment_src.has_value()) {
            fmt::println(stderr, "Failed to open read shader file.");
            return std::nullopt;
        }
        auto const vertex = create_shader(vertex_src->c_str(), GL_VERTEX_SHADER);
        if (!vertex.has_value()) {
            fmt::println("vertex fail");
            return std::nullopt;
        }
        auto const fragment = create_shader(fragment_src->c_str(), GL_FRAGMENT_SHADER);
        if (!fragment.has_value()) {
            fmt::println("fragment fail");
            glDeleteShader(vertex.value());
            return std::nullopt;
        }

        auto const id = glCreateProgram();
        glAttachShader(id, vertex.value());
        glAttachShader(id, fragment.value());
        glLinkProgram(id);
        {
            GLint success = 0;
            glGetProgramiv(id, GL_LINK_STATUS, &success);
            if (success == 0) {
                auto info_log = std::array<char, 512U>{};  // NOLINT
                glGetProgramInfoLog(id, info_log.size(), nullptr, info_log.data());
                fmt::println(stderr, "Program link error: {}", info_log.data());
                glDeleteShader(vertex.value());
                glDeleteShader(fragment.value());
                return std::nullopt;
            }
        }
        glDeleteShader(vertex.value());
        glDeleteShader(fragment.value());

        return Shader{id};
    }

    Shader(Shader const &) = delete;
    auto operator=(Shader const &) -> Shader & = delete;

    Shader(Shader &&other) noexcept
        : m_id{std::exchange(other.m_id, std::nullopt)} {
    }

    auto operator=(Shader &&other) noexcept -> Shader & {
        m_id = std::exchange(other.m_id, std::nullopt);
        return *this;
    }

    ~Shader() {
        if (m_id.has_value()) {
            glDeleteProgram(m_id.value());
        }
    }

private:
    explicit Shader(GLuint id)
        : m_id{id} {
    }

    std::optional<GLuint> m_id;
};

#endif
