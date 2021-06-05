#version 450


vec2 positions[3] = vec2[] (
    vec2(0.1, -0.2),
    vec2(0.8, 0.5),
    vec2(-0.3, -0.8)
);

void main() {
    // output in gl_Position (4d vector)
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}