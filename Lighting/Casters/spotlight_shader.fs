#version 330 core

out vec4 FragColor;

in vec3 fPos;
in vec3 fNor;
in vec2 fCoord;

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
    vec3 direction;
    float cutoff;
    float outcutoff;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 camPos;

uniform Material material;
uniform Light light;

void main() {
    vec3 texdiffuse = texture(material.diffuse, fCoord).rgb;
    vec3 texspecular = texture(material.specular, fCoord).rgb;
    // ambient
    vec3 ambient = light.ambient * texdiffuse;

    vec3 litdir = normalize(light.position-fPos);
    float theta = dot(litdir, normalize(-light.direction));
    vec3 norm = normalize(fNor);
    // diffuse
    vec3 diffuse = light.diffuse * max(dot(litdir, norm), 0.0) * texdiffuse;
    // specular
    vec3 camdir = normalize(camPos-fPos);
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = light.specular * pow(max(dot(camdir, refdir), 0.0), material.shininess) * texspecular;

    float distance = length(light.position-fPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float intensity = clamp((theta - light.outcutoff)/(light.cutoff-light.outcutoff), 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
