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
    vec3 direction;

    vec3 position;
    float constant;
    float linear;
    float quadratic;
};

in vec3 fPos;
in vec3 fNor;
in vec2 fCoord;

uniform Material material;
uniform Light light;
uniform vec3 camPos;

void main() {
    vec3 texdiffuse = texture(material.diffuse, fCoord).rgb;
    vec3 texspecular = texture(material.specular, fCoord).rgb;
    // ambient
    vec3 ambient = light.ambient * texdiffuse;
    // diffuse
    // vec3 litdir = normalize(-light.direction);
    vec3 litdir = normalize(light.position-fPos);
    vec3 norm = normalize(fNor);
    vec3 diffuse = light.diffuse * max(dot(litdir, norm), 0.0) * texdiffuse;
    // specular
    vec3 camdir = normalize(camPos-fPos);
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = light.specular * pow(max(dot(camdir, refdir), 0.0), material.shininess) * texspecular;

    float distance = length(light.position-fPos);
    float attenuation = 1.0/(light.constant+light.linear*distance+light.quadratic*distance*distance);

    FragColor = vec4((ambient+diffuse+specular)*attenuation, 1.0);
}
