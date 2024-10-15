#version 330 core

out vec4 FragColor;

void main() {
    if (gl_FragCoord.x > 400) {
        FragColor = vec4(1.f, 0.f, 0.f, 1.f);
    } else {
        FragColor = vec4(0.f, 1.f, 0.f, 1.f);
    }
}
