#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    // vec3 specular;
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
uniform vec3 camPos;

in vec3 fNor;
in vec3 fPos;
in vec2 fCoord;

void main() {
    vec3 texturediff = texture(material.diffuse, fCoord).rgb;
    vec3 texturespec = texture(material.specular, fCoord).rgb;
    // ambient
    vec3 ambcolor = light.ambient * texturediff;
    // diffuse
    vec3 norm = normalize(fNor);
    vec3 litdir = normalize(light.position-fPos);
    vec3 diffusecolor = light.diffuse * max(dot(norm, litdir), 0.0) * texturediff;
    // specular
    vec3 camdir = normalize(camPos-fPos);
    vec3 refdir = reflect(-litdir, norm);
    vec3 specularcolor = light.specular * (pow(max(dot(camdir, refdir), 0.0), material.shininess) * texturespec);

    vec3 retColor = ambcolor + diffusecolor + specularcolor;
    FragColor = vec4(retColor, 1.0);
}
