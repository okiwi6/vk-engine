#version 450

// attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec3 frag_pos_world; 
layout(location = 2) out vec3 frag_normal_world;

layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 projection_view_matrix;
    vec4 ambient_light_color;
    vec3 light_position;
    vec4 light_color;
} ubo;

layout(push_constant) uniform Push {
    mat4 model_matrix; // projection * view * model
    mat4 normal_mat; // model
} push;


void main() {
    vec4 position_world = push.model_matrix * vec4(position, 1.0);
    gl_Position = ubo.projection_view_matrix * position_world;


    frag_normal_world = normalize(mat3(push.normal_mat) * normal);
    frag_pos_world = position_world.xyz;
    frag_color = color;
}