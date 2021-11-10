#version 450

// attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 projection_view_matrix;
    vec3 direction_to_light;
} ubo;

layout(push_constant) uniform Push {
    mat4 model_matrix; // projection * view * model
    mat4 normal_mat; // model
} push;

const float AMBIENT = 0.02;

void main() {
    
    gl_Position = ubo.projection_view_matrix * push.model_matrix * vec4(position, 1.0);


    vec3 normal_world_space = normalize(mat3(push.normal_mat) * normal);

    float light_intensity = max(dot(normal_world_space, ubo.direction_to_light), 0) + AMBIENT;

    fragColor = color * light_intensity;
}