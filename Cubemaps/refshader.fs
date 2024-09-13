#version 330 core

out vec4 FragColor;

// uniform sampler2D texcontainer;
in vec3 Normal;
in vec3 Position;
uniform vec3 cameraPos;
uniform samplerCube skybox;

// in vec2 TexCoords;

void main() {
    // FragColor = texture(texcontainer, TexCoords);
    vec3 I = normalize(Position-cameraPos);
    // 反射
    // vec3 R = reflect(I, normalize(Normal));
    // refraction - 折射
    float ratio = 1.0 / 1.52;
    vec3 Rerfr = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, Rerfr).rgb, 1.0);
}
