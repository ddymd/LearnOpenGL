#version 330 core

out vec4 FragColor;

in vec3 fPos;
in vec3 fNor;
in vec3 fLitPos;

uniform vec3 objColor;
uniform vec3 litColor;

float ambStren = 0.1;
float specularStren = 0.5;

void main() {
    vec3 ambColor = ambStren * litColor;

    vec3 norm = normalize(fNor);
    vec3 litDir = normalize(fLitPos - fPos);
    vec3 diffuseColor = max(dot(norm, litDir), 0.0) * litColor;

    vec3 viewDir = normalize(-fPos);
    vec3 reflectDir = reflect(-litDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);
    vec3 specColor = specularStren*spec*litColor;

    vec3 retColor = (ambColor + diffuseColor + specColor) * objColor;
    FragColor = vec4(retColor, 1.0);
}
