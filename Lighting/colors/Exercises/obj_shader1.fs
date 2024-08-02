#version 330 core

out vec4 FragColor;

in vec3 ourNor;

uniform vec3 objColor;
uniform vec3 litColor;

uniform vec3 litPos;
in vec3 fragPos;

uniform vec3 viewPos;

float ambStrength = 0.1;

float specularStrength = 0.5;
void main() {
    vec3 norm = normalize(ourNor);
    // 环境光
    vec3 ambColor = ambStrength * litColor;
    // 漫反射光
    vec3 litDir = normalize(litPos-fragPos);
    vec3 diffuseColor = max(dot(norm, litDir), 0.0) * litColor;
    // 镜面反射光
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-litDir, norm);

    vec3 specularColor = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32) * litColor;

    vec3 retColor = (ambColor+diffuseColor+specularColor) * objColor;
    FragColor = vec4(retColor, 1.0);
}
