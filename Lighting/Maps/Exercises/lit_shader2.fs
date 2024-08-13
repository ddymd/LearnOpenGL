#version 330 core

out vec4 FragColor;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;


void main() {
    FragColor = vec4(vec3(1.0), 1.0);
}
