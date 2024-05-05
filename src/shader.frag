#version 330 core

out vec4 FragColor;
in vec4 gl_FragCoord;
uniform vec2 view_port;
uniform float x_offset;
uniform float y_offset;
uniform float zoom;

#define MAX_ITERS 1000U
#define THRESHOLD 4.0f

float square(float x) {
    return x * x;
}

float sq_abs_val(float real, float imag) {
    return square(real) + square(imag);
}

uint calc_iters(float real, float imag) {
    float ref_real = real;
    float ref_imag = imag;
    uint iterations = 0U;

    while ((iterations < MAX_ITERS) && (sq_abs_val(real, imag) <= THRESHOLD)) {
        float temp = square(real) - square(imag) + ref_real;
        imag = 2.0f * real * imag + ref_imag;
        real = temp;
        ++iterations;
    }
    return iterations;
}

vec2 real_imag() {
    return (((gl_FragCoord.xy / view_port - vec2(0.7f, 0.5f)) * zoom) + vec2(x_offset, y_offset)) * 4.0f;
}

vec3 rainbow(float val) {
    if (val == 1.0f) {
        val -= 0.0001f;
    }
    const float m = 0.25f;
    uint num = uint(val / m);
    if (num > 3U) {
        num = 3U;
    }
    float s = (val - float(num) * m) / m;

    switch (num) {
        case 0U:
        return vec3(0.0f, s, 1.0f);
        case 1U:
        return vec3(0.0f, 1.0f, 1.0f - s);
        case 2U:
        return vec3(s, 1.0f, 0.0f);
        case 3U:
        return vec3(1.0f, 1.0f - s, 0.0f);
        default:
        return vec3(0.0f, 0.0f, 0.0f);
    }
}

void main() {
    vec2 normalized_xy = real_imag();
    uint iterations = calc_iters(normalized_xy.x, normalized_xy.y);
    if (iterations == MAX_ITERS) {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0);
    } else {
        vec3 color = rainbow(float(iterations) / MAX_ITERS);
        FragColor = vec4(color, 1.0);
    }
}
