#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
// layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

// out vec2 TexCoords;
out vec3 Normal;
out vec3 Position;

void main() {
    // TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNor;
    Position = vec3(model*vec4(aPos, 1.0));
    gl_Position = proj * view * vec4(Position, 1.0);
}
