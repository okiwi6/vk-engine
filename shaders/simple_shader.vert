#version 450

// attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
    mat4 transform; // projection * view * model
    mat4 normal_mat; // model
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.02;

void main() {
    
    gl_Position = push.transform * vec4(position, 1.0);


    vec3 normal_world_space = normalize(mat3(push.normal_mat) * normal);

    float light_intensity = max(dot(normal_world_space, DIRECTION_TO_LIGHT), 0) + AMBIENT;

    fragColor = color * light_intensity;
}