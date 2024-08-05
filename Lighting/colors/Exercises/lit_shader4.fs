#version 330 core

out vec4 FragColor;

uniform vec3 litColor;

void main() {
    FragColor = vec4(litColor, 1.0);
}
