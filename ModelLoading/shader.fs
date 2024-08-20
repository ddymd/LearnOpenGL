#version 330 core

out vec4 FragColor;

in vec3 fPos;
in vec3 fNor;
in vec2 fCoord;

struct Material {
    sampler2D texdiffuse1;
    sampler2D texspecular1;
    float shininess;
};

struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float innercutoff;
    float outercutoff;
};

uniform Material material;
uniform vec3 camPos;
uniform DirectionalLight dirlight;
uniform SpotLight spotlight;

vec3 CalcDirectionalLight(DirectionalLight lit, vec3 texdiffuse, vec3 texspecular, vec3 norm, vec3 camdir, float shininess) {
    vec3 ambient = lit.ambient * texdiffuse;
    vec3 litdir = normalize(lit.direction);
    vec3 diffuse = max(dot(-litdir, norm), 0.0) * texdiffuse;
    vec3 refdir = reflect(litdir, norm);
    vec3 specular = pow(max(dot(litdir, camdir), 0.0), shininess) * texspecular;
    return ambient+diffuse+specular;
}

vec3 CalcSpotLight(SpotLight lit, vec3 texdiffuse, vec3 texspecular, vec3 fpos, vec3 norm, vec3 camdir, float shininess) {
    vec3 ambient = lit.ambient * texdiffuse;
    vec3 litdir = normalize(lit.position-fpos);
    vec3 diffuse = max(dot(litdir, norm), 0.0) * texdiffuse;
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = pow(max(dot(litdir, camdir), 0.0), shininess) * texspecular;
    float distance = length(lit.position-fpos);
    float attenuation = 1.0/(lit.constant+lit.linear*distance+lit.quadratic*distance*distance);
    float theta = dot(-litdir, normalize(lit.direction));
    float intensity = clamp((theta-lit.outercutoff)/(lit.innercutoff-lit.outercutoff), 0.0, 1.0);
    return (ambient+diffuse+specular)*attenuation*intensity;
}

void main() {
    vec3 texdiffuse = texture(material.texdiffuse1, fCoord).rgb;
    vec3 texspecular = texture(material.texspecular1, fCoord).rgb;
    vec3 norm = normalize(fNor);
    vec3 camdir = normalize(camPos - fPos);
    vec3 dircolor = CalcDirectionalLight(dirlight, texdiffuse, texspecular, norm, camdir, material.shininess);
    vec3 spotcolor = CalcSpotLight(spotlight, texdiffuse, texspecular, fPos, norm, camdir, material.shininess);

    FragColor = vec4(dircolor+spotcolor, 1.0);
}
