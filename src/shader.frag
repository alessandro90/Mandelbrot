#version 330 core
out vec4 FragColor;
in vec4 gl_FragCoord;

void main() {
    float x = gl_FragCoord.x / 800;
    float y = gl_FragCoord.y / 600;
    FragColor = vec4(x, y, 0.0f, 1.0);
}
