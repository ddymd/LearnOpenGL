#version 330 core

out vec4 FragColor;

in vec3 fColor;

uniform vec3 objColor;

void main() {
    FragColor = vec4(fColor*objColor, 1.0);
}
