#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

uniform vec3 litColor;
// uniform vec3 objColor;

// uniform vec3 litPos;
uniform vec3 camPos;

uniform Material material;
uniform Light light;

in vec3 fNor;
in vec3 fPos;

// float ambStren = 0.1;
// float respStren = 0.5;
void main() {
    vec3 ambColor = light.ambient * material.ambient;

    vec3 norm = normalize(fNor);
    vec3 litDir = normalize(light.position - fPos);
    float diffuse = max(dot(litDir, norm), 0.0);
    vec3 diffuseColor = light.diffuse * (diffuse * material.diffuse);

    vec3 refDir = reflect(-litDir, norm);
    vec3 camDir = normalize(camPos - fPos);
    float respec = pow(max(dot(camDir, refDir), 0.0), material.shininess);
    vec3 respecularColor = light.specular * (respec * material.specular);

    // vec3 retColor = (ambColor + diffuseColor + respecularColor) * objColor;
    vec3 retColor = ambColor + diffuseColor + respecularColor;
    FragColor = vec4(retColor, 1.0);
}
