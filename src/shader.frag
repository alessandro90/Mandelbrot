#version 330 core

out vec4 FragColor;
in vec4 gl_FragCoord;
uniform vec2 view_port;

#define MAX_ITERS 500U
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

void main() {
    vec2 normalized_xy = (gl_FragCoord.xy / view_port - vec2(0.7f, 0.5f)) * 4.0f;
    uint iterations = calc_iters(normalized_xy.x, normalized_xy.y);
    if (iterations == MAX_ITERS) {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0);
    } else {
        float green = float(iterations) / MAX_ITERS;
        FragColor = vec4(0.0f, green, 0.0f, 1.0);
    }
}
