#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "glad/glad.h"
#include <initializer_list>
#include <optional>
#include <string_view>
#include <utility>

namespace gl {
class Program {
public:
    [[nodiscard]] static auto create_and_link(std::initializer_list<std::string_view> shader_paths)
        -> std::optional<Program>;

    Program(Program const &) = delete;
    auto operator=(Program const &) -> Program & = delete;
    Program(Program &&) noexcept;
    auto operator=(Program &&) noexcept -> Program &;

    ~Program();

    auto use() const -> void;

    template <typename T>
    auto set_uniform(std::string_view name, T value) const -> void {
        auto const loc = get_uniform_location(name);
        if constexpr (std::is_same_v<T, float>) {
            glUniform1f(loc, value);
        } else if constexpr (std::is_same_v<T, GLint>) {
            glUniform1i(loc, value);
        } else if constexpr (std::is_same_v<T, std::pair<float, float>>) {
            glUniform2f(loc, value.first, value.second);
        } else {
            static_assert(false, "Unsupported uniform value");
        }
    }

private:
    inline static constexpr auto s_invalid_program_id = GLint{0};

    explicit Program(GLuint prog_id)
        : m_prog_id{prog_id} {
    }

    [[nodiscard]] auto get_uniform_location(std::string_view name) const -> GLint;

    GLuint m_prog_id;
};

}  // namespace gl
#endif
