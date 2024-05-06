#ifndef APP_HPP
#define APP_HPP

#include "glad/glad.h"

#include <filesystem>

class App {
public:
    auto run(std::filesystem::path shaders_path) -> void;

private:
    inline static constexpr auto s_default_x_offset = 0.0F;
    inline static constexpr auto s_default_y_offset = 0.0F;
    inline static constexpr auto s_default_zoom = 1.0F;

    float m_x_offset{s_default_x_offset};
    float m_y_offset{s_default_y_offset};
    float m_zoom{s_default_zoom};
    GLuint m_color_map{};

    bool m_space_key_is_pressed{false};

    [[nodiscard]] auto scaling_factor() const -> float;
};

#endif
