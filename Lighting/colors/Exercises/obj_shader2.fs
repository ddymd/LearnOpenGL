#version 330 core
out vec4 FragColor;

in vec3 ourPos;
in vec3 ourNor;

uniform vec3 objColor;
uniform vec3 litColor;

uniform vec3 litPos;
uniform vec3 viewPos;

float ambStrength = 0.1;
float diffuseStrg = 0.5;
float specularStrg = 0.5;
void main() {
    vec3 ambColor = ambStrength * litColor;

    vec3 norm = normalize(ourNor);
    vec3 litDir = normalize(litPos - ourPos);
    vec3 diffuseColor = diffuseStrg * max(dot(norm, litDir), 0.0) * litColor;

    vec3 viewDir = normalize(viewPos - ourPos);
    vec3 reflectDir = reflect(-litDir, norm);

    vec3 specularColor = specularStrg * pow(max(dot(viewDir, reflectDir), 0.0), 32) * litColor;

    vec3 retColor = (ambColor + diffuseColor + specularColor) * objColor;
    FragColor = vec4(retColor, 1.0);
}

