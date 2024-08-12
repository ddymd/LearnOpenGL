#version 330 core
out vec4 FragColor;

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

uniform sampler2D texture0;
uniform Material material;
uniform Light light;
uniform vec3 camPos;

in vec3 fNor;
in vec3 fPos;
in vec2 aCoord;

void main() {
    vec3 tt = texture(texture0, aCoord).rgb;

    // ambient
    vec3 ambcolor = light.ambient * tt;
    // diffuse
    vec3 norm = normalize(fNor);
    vec3 litdir = normalize(light.position-fPos);
    vec3 diffusecolor = light.diffuse * max(dot(norm, litdir), 0.0) * tt;
    // specular
    vec3 camdir = normalize(camPos-fPos);
    vec3 refdir = reflect(-litdir, norm);
    vec3 specularcolor = light.specular * (pow(max(dot(camdir, refdir), 0.0), material.shininess) * material.specular);

    vec3 retColor = ambcolor + diffusecolor + specularcolor;

    // FragColor = texture(texture0, aCoord) * vec4(retColor, 1.0);
    FragColor = vec4(retColor, 1.0);
}
