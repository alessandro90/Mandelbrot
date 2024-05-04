#version 330 core

out vec4 FragColor;
in vec4 gl_FragCoord;
uniform vec2 view_port;

#define MAX_ITERS 500
#define THRESHOLD 4.0f;

float square(float x) {
    return x * x;
}

float sq_abs_val(float real, float imag) {
    return square(real) + square(imag);
}

uint calc_iters(float real, float imag) {
    const float ref_real = real;
    const float ref_imag = imag;
    uint iterations = 0;

    while (iterations < MAX_ITERS && sq_abs_val(real, imag) <= THRESHOLD) {
        const float temp = square(real) - square(y) + ref_real;
        y = 2.0f * real * imag + ref_imag;
        real = temp;
        iterations += 1;
    }
    return iterations;
}

void main() {
    vec2 normalized_xy = (gl_FragCoord.xy / view_port - vec2(-0.5f, -0.7f)) * 4.0f;
    const uint iterations = calc_iters(normalized_xy.x, normalized_xy.y);
    if (iterations == MAX_ITERS) {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0);
    } else {
        const float green = float(iterations) / MAX_ITERS;
        FragColor = vec4(0.0f, green, 0.0f, 1.0);
    }
}
