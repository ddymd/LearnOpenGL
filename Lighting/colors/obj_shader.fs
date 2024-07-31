#version 330 core

out vec4 FragColor;
in vec3 ourNor;
in vec2 ourCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 objcolor;
uniform vec3 litcolor;
// 光源位置
uniform vec3 litpos;
// 相机位置/视角位置
uniform vec3 viewpos;

in vec3 fragpos;

void main() {
    // ambient lighting - 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * litcolor;
    // diffuse lighting - 漫反射
    vec3 norm = normalize(ourNor);
    vec3 litdir = normalize(litpos - fragpos);
    float diff = max(dot(norm, litdir), 0.0);
    vec3 diffuse = diff * litcolor;
    // specular lighting - 镜面反射
    float specularStrength = 0.5;
    vec3 viewdir = normalize(viewpos - fragpos);
    vec3 reflectdir = reflect(-litdir, norm);
    float spec = pow(max(dot(viewdir, reflectdir), 0.0), 32);
    vec3 specular = specularStrength * spec * litcolor;

    vec3 result = (ambient + diffuse + specular) * objcolor;
    FragColor = vec4(result, 1.0);
}

