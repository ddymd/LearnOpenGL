#version 330 core

out vec4 FragColor;

in vec2 fCoord;
uniform sampler2D texture0;
uniform sampler2D texture1;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far+near-z*(far-near));
}

void main() {
    // =====> 1
    FragColor = texture(texture0, fCoord);
    // =====> 2
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    // =====> 3
    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // FragColor = vec4(vec3(depth), 1.0);
}
