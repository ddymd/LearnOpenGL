#version 330 core

out vec4 FragColor;

uniform vec3 litColor;
uniform vec3 objColor;

uniform vec3 litPos;
uniform vec3 camPos;

in vec3 fNor;
in vec3 fPos;

float ambStren = 0.1;
float respStren = 0.5;
void main() {
    vec3 ambColor = ambStren * litColor;

    vec3 norm = normalize(fNor);
    vec3 litDir = normalize(litPos - fPos);
    float diffuseStren = max(dot(litDir, norm), 0.0);
    vec3 diffuseColor = diffuseStren * litColor;

    vec3 refDir = reflect(-litDir, norm);
    vec3 camDir = normalize(camPos - fPos);
    float respecularStren = pow(max(dot(camDir, refDir), 0.0), 32);
    vec3 respecularColor = respStren * respecularStren * litColor;

    vec3 retColor = (ambColor + diffuseColor + respecularColor) * objColor;
    FragColor = vec4(retColor, 1.0);
}

