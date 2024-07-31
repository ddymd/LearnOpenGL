#version 330 core

out vec4 FragColor;

uniform vec3 objColor;
uniform vec3 litColor;

float ambientStrength = 0.2;

void main() {
    vec3 ambient = ambientStrength * litColor;
    FragColor = vec4(ambient*objColor, 1.0);
}
