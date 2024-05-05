#include "program.hpp"

#include "fmt/base.h"
#include "glad/glad.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace {
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

class Shader {
public:
    explicit Shader(GLuint id)
        : m_id{id} {
    }

    [[nodiscard]] auto id() const -> GLuint {
        return m_id.value();
    }

    Shader(Shader const &) = delete;
    auto operator=(Shader const &) -> Shader & = delete;

    Shader(Shader &&other) noexcept
        : m_id{std::exchange(other.m_id, std::nullopt)} {
    }

    [[nodiscard]] auto operator=(Shader &&other) noexcept -> Shader & {
        m_id = std::exchange(other.m_id, std::nullopt);
        return *this;
    }

    ~Shader() {
        if (m_id.has_value()) {
            glDeleteShader(m_id.value());
        }
    }

private:
    std::optional<GLuint> m_id;
};

[[nodiscard]] inline auto create_shader(const char *src, int type) -> std::optional<Shader> {
    auto const shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    if (!check_shader_compile_error(shader, "Shader error")) {
        return std::nullopt;
    }
    return Shader{shader};
}

[[nodiscard]] auto get_shader_type(std::string_view path) -> int {
    if (path.ends_with(".vert")) {
        return GL_VERTEX_SHADER;
    }

    if (path.ends_with(".frag")) {
        return GL_FRAGMENT_SHADER;
    }

    fmt::println(stderr,
                 "Invalid shader type. Unsupported extension: {}.\n Supported extensions are: "
                 "'.vert', '.frag'.",
                 path);
    std::abort();
}


}  // namespace

namespace gl {

Program::Program(Program &&other) noexcept
    : m_prog_id{std::exchange(other.m_prog_id, s_invalid_program_id)} {
}

auto Program::operator=(Program &&other) noexcept -> Program & {
    m_prog_id = std::exchange(other.m_prog_id, s_invalid_program_id);
    return *this;
}

auto Program::create_and_link(std::initializer_list<std::string_view> shader_paths)
    -> std::optional<Program> {
    auto const prog_id = glCreateProgram();
    for (auto shader_path : shader_paths) {
        auto const src = read_file(shader_path);
        if (!src.has_value() || !src.has_value()) {
            fmt::println(stderr, "Failed to open read shader file {}.", shader_path);
            glDeleteProgram(prog_id);
            return std::nullopt;
        }
        auto const shader_type = get_shader_type(shader_path);
        auto const shader = create_shader(src->c_str(), shader_type);
        if (!shader.has_value()) {
            glDeleteProgram(prog_id);
            return std::nullopt;
        }
        glAttachShader(prog_id, shader.value().id());
    }
    glLinkProgram(prog_id);
    {
        GLint success = 0;
        glGetProgramiv(prog_id, GL_LINK_STATUS, &success);
        if (success == 0) {
            auto info_log = std::array<char, 512U>{};  // NOLINT
            glGetProgramInfoLog(prog_id, info_log.size(), nullptr, info_log.data());
            fmt::println(stderr, "Program link error: {}", info_log.data());
            glDeleteProgram(prog_id);
            return std::nullopt;
        }
    }
    return Program{prog_id};
}

auto Program::use() const -> void {
    glUseProgram(m_prog_id);
}

auto Program::get_uniform_location(std::string_view name) const -> GLint {
    auto const loc = glGetUniformLocation(m_prog_id, name.data());
    if (loc == -1) {
        fmt::println(stderr, "Invalid uniform name: {}", name);
        std::abort();
    }
    return loc;
}

Program::~Program() {
    if (m_prog_id != s_invalid_program_id) {
        glDeleteProgram(m_prog_id);
    }
}
}  // namespace gl
