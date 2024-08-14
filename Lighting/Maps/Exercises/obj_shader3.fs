#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 fCoord;

uniform Material material;
uniform Light light;
uniform vec3 camPos;

void main() {
    vec3 texdiffuse = texture(material.diffuse, fCoord).rgb;
    vec3 texspecular = texture(material.specular, fCoord).rgb;
    vec3 texemission = texture(material.emission, fCoord).rgb;

    // ambient
    vec3 ambient = light.ambient * texdiffuse;
    // diffuse
    vec3 norm = normalize(fNor);
    vec3 litdir = normalize(light.position-fPos);
    vec3 diffuse = light.diffuse * max(dot(litdir, norm), 0.0) * texdiffuse;
    // specular
    vec3 camdir = normalize(camPos-fPos);
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = light.specular * pow(max(dot(camdir, refdir), 0.0), material.shininess) * texspecular;

    FragColor = vec4(ambient+diffuse+specular+texemission, 1.0);
}
