#version 330 core

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

uniform Light light;

void main() {
    FragColor = vec4(light.ambient + light.diffuse + light.specular, 1.0);
}
