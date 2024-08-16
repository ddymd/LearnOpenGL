#version 330 core

out vec4 FragColor;

uniform vec3 litcolor;

void main() {
    FragColor = vec4(litcolor, 1.0);
}
