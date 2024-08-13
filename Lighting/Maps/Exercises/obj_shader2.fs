#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

uniform Material material;
uniform Light light;

in vec3 fPos;
in vec3 fNor;
in vec2 fCoord;

uniform vec3 camPos;

void main() {
    vec3 diffusetex = texture(material.diffuse, fCoord).rgb;
    vec3 speculartex = texture(material.specular, fCoord).rgb;

    // ambient
    vec3 ambient = light.ambient * diffusetex;
    // diffuse
    vec3 norm = normalize(fNor);
    vec3 litdir = normalize(light.position-fPos);
    vec3 diffuse = max(dot(litdir, norm), 0.0) * diffusetex;

    // specular
    vec3 camdir = normalize(camPos-fPos);
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = pow(max(dot(camdir, refdir), 0.0), material.shininess) * speculartex;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
