#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 litColor;
// uniform vec3 objColor;

uniform vec3 litPos;
uniform vec3 camPos;

out vec3 fColor;

float ambStrength = 0.1;
float specStrength = 1.0;
void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0);

    // ambient lighting
    vec3 ambColor = ambStrength * litColor;
    // diffuse lighting
    vec3 pos = vec3(model * vec4(aPos, 1.0));
    vec3 norm = normalize(mat3(transpose(inverse(model))) * aNor);
    vec3 litDir = normalize(litPos - pos);
    vec3 diffuseColor = max(dot(norm, litDir), 0.0) * litColor;
    // specular lighting
    vec3 camDir = normalize(camPos - pos);
    vec3 reflectDir = reflect(-litDir, norm);
    vec3 specColor = specStrength * pow(max(dot(camDir, reflectDir), 0.0), 32) * litColor;

    // fColor = (ambColor + diffuseColor + specColor) * objColor;
    fColor = ambColor + diffuseColor + specColor;
}
