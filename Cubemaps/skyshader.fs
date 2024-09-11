#version 330 core

out vec4 FragColor;

uniform samplerCube texcube;

in vec3 TexCoords;

void main() {
    FragColor = texture(texcube, TexCoords);
}
