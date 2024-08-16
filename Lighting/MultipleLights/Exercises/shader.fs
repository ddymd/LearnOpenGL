#version 330 core

#define NR_POINT_LIGHTS 4

out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 fCoord;
uniform vec3 camPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

struct LightDirectional {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform LightDirectional litdirectional;

struct LightPoint {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
uniform LightPoint litpoint[NR_POINT_LIGHTS];

struct LightSpot {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float innercutoff;
    float outercutoff;
};
uniform LightSpot litspot;


vec3 CalculateDirectionalColor(LightDirectional litght, vec3 texdiffuse, vec3 texspecular, vec3 norm, vec3 camdir) {
    vec3 ambient = litght.ambient * texdiffuse;
    vec3 litdir = normalize(litght.direction);
    vec3 diffuse = litght.diffuse * max(dot(-litdir, norm), 0.0) * texdiffuse;
    vec3 refdir = reflect(litdir, norm);
    vec3 specular = litght.specular * pow(max(dot(refdir, camdir), 0.0), material.shininess) * texspecular;
    return ambient+diffuse+specular;
}

vec3 CalculatePointLightColor(LightPoint light, vec3 texdiffuse, vec3 texspecular, vec3 norm, vec3 camdir, vec3 fpos) {
    vec3 ambient = light.ambient * texdiffuse;
    vec3 litdir = normalize(light.position-fpos);
    vec3 diffuse = light.diffuse * max(dot(litdir, norm), 0.0) * texdiffuse;
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = light.specular * pow(max(dot(refdir, camdir), 0.0), material.shininess) * texspecular;
    float distance = length(light.position-fPos);
    float attenuation = 1.0/(light.constant+light.linear*distance+light.quadratic*distance*distance);
    return (ambient+diffuse+specular)*attenuation;
}

vec3 CalculateSpotlightColor(LightSpot light, vec3 texdiffuse, vec3 texspecular, vec3 norm, vec3 camdir, vec3 fpos) {
    vec3 ambient = light.ambient * texdiffuse;
    vec3 litdir = normalize(light.position-fpos);
    vec3 diffuse = light.diffuse * max(dot(litdir, norm), 0.0) * texdiffuse;
    vec3 refdir = reflect(-litdir, norm);
    vec3 specular = light.specular * pow(max(dot(refdir, camdir), 0.0), material.shininess) * texspecular;
    float distance = length(light.position-fPos);
    float attenuation = 1.0/(light.constant+light.linear*distance+light.quadratic*distance*distance);
    float theta = dot(litdir, normalize(-light.direction));
    float intensity = clamp((theta-light.outercutoff)/(light.innercutoff-light.outercutoff), 0.0, 1.0);
    return (ambient+diffuse+specular)*attenuation*intensity;
}

void main() {
    vec3 texdiffuse = texture(material.diffuse, fCoord).rgb;
    vec3 texspecular = texture(material.specular, fCoord).rgb;
    vec3 norm = normalize(fNor);
    vec3 camdir = normalize(camPos-fPos);
    // calculate directional light color
    vec3 dcolor = CalculateDirectionalColor(litdirectional, texdiffuse, texspecular, norm, camdir);
    // calclate point light color
    vec3 pcolor = vec3(0.0);
    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        pcolor += CalculatePointLightColor(litpoint[3], texdiffuse, texspecular, norm, camdir, fPos);
    }
    // calculate spot light color
    vec3 scolor = CalculateSpotlightColor(litspot, texdiffuse, texspecular, norm, camdir, fPos);

    FragColor = vec4(dcolor+pcolor+scolor, 1.0);
}
