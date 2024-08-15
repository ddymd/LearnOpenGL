#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct LightDirectional {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightPoint {
    float constant;
    float linear;
    float quadratic;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightSpotlight {
    float constant;
    float linear;
    float quadratic;
    float innercutoff;
    float outercutoff;
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 fPos;
in vec3 fNor;
in vec2 fCoord;

uniform vec3 camPos;
uniform Material material;
uniform LightDirectional litdire;
#define NR_POINTER_LIGHT 4
uniform LightPoint litpoint[NR_POINTER_LIGHT];
uniform LightSpotlight litspot;

vec3 calcDirectionalLight(LightDirectional lit, vec3 texdiffuse, vec3 texspecular, vec3 norm, vec3 camdir, float shininess) {
    // ambient
    vec3 ambient = lit.ambient * texdiffuse;
    // diffuse
    vec3 litdir = normalize(-lit.direction);
    vec3 diffuse = lit.diffuse * max(dot(litdir, norm), 0.0) * texdiffuse;
    // specular
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = lit.specular * pow(max(dot(camdir, refdir), 0.0), shininess) * texspecular;
    return ambient + diffuse + specular;
}

vec3 calcPointLight(LightPoint lit, vec3 texdiffuse, vec3 texspecular, vec3 fragpos, vec3 norm, vec3 camdir, float shininess) {
    // attenuation
    float distance = length(fragpos-lit.position);
    float attenuation = 1.0/(lit.constant+lit.linear*distance+lit.quadratic*distance*distance);
    // ambient
    vec3 ambient = lit.ambient * texdiffuse;
    // diffuse
    vec3 litdir = normalize(lit.position-fragpos);
    vec3 diffuse = lit.diffuse * max(dot(litdir, norm), 0.0) * texdiffuse;
    // specular
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = lit.specular * pow(max(dot(camdir, refdir), 0.0), shininess) * texspecular;
    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(LightSpotlight lit, vec3 texdiffuse, vec3 texspecular, vec3 fragpos, vec3 norm, vec3 camdir, float shininess) {
    // attenuation
    float distance = length(fragpos-lit.position);
    float attenuation = 1.0/(lit.constant+lit.linear*distance+lit.quadratic*distance*distance);
    // ambient
    vec3 ambient = lit.ambient * texdiffuse;
    // diffuse
    vec3 litdir = normalize(lit.position-fragpos);
    vec3 diffuse = lit.diffuse * max(dot(litdir, norm), 0.0) * texdiffuse;
    // specular
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = lit.specular * pow(max(dot(camdir, refdir), 0.0), shininess) * texspecular;

    float theta = dot(litdir, normalize(-lit.direction));
    float intensity = clamp((theta-lit.outercutoff)/(lit.innercutoff-lit.outercutoff), 0.0, 1.0);
    return (ambient + diffuse + specular) * intensity * attenuation;
}

void main() {
    vec3 texdiffuse = texture(material.diffuse, fCoord).rgb;
    vec3 texspecular = texture(material.specular, fCoord).rgb;
    vec3 norm = normalize(fNor);
    vec3 camdir = normalize(camPos-fPos);
    vec3 dircolor = calcDirectionalLight(litdire, texdiffuse, texspecular, norm, camdir, material.shininess);
    vec3 pointcolor = vec3(0.0);
    for (int i = 0; i < NR_POINTER_LIGHT; ++i) {
        pointcolor += calcPointLight(litpoint[i], texdiffuse, texspecular, fPos, norm, camdir, material.shininess);
    }
    vec3 spotcolor = calcSpotLight(litspot, texdiffuse, texspecular, fPos, norm, camdir, material.shininess);

    FragColor = vec4(dircolor+pointcolor+spotcolor, 1.0);
}
