#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

out vec4 FragColor;
uniform Material material;
uniform Light light;

uniform vec3 camPos;

in vec3 fNor;
in vec3 fPos;

void main() {
    // ambient
    vec3 ambColor = material.ambient * light.ambient;

    // diffuse
    vec3 norm = normalize(fNor);
    vec3 litdir = normalize(light.position - fPos);
    vec3 diffuseColor = max(dot(litdir, norm), 0.0) * material.diffuse * light.diffuse;

    // specular
    vec3 camdir = normalize(camPos - fPos);
    vec3 refdir = reflect(-litdir, norm);
    vec3 specularColor = pow(max(dot(refdir, camdir), 0.0), material.shininess) * material.specular * light.specular;

    vec3 retColor = ambColor + diffuseColor + specularColor;
    FragColor = vec4(retColor, 1.0);
}

