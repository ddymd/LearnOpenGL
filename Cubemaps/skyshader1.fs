#version 330 core

out vec4 FragColor;

in vec3 TexCoords;
uniform samplerCube texcube;

void main() {
    FragColor = texture(texcube, TexCoords);
}