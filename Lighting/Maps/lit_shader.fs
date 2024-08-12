#version 330 core

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;
uniform Light light;

void main() {
    vec3 retColor = light.ambient + light.diffuse + light.specular;
    FragColor = vec4(retColor, 1.0);
}
