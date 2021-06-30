#version 450

// attributes
layout(location = 0) in vec2 position;

void main() {
    // output in gl_Position (4d vector)
    gl_Position = vec4(position, 0.0, 1.0);
}