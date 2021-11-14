#version 450

// attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

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


    vec3 normal_world_space = normalize(mat3(push.normal_mat) * normal);
    
    vec3 direction_to_light = (ubo.light_position - position_world.xyz);
    float attenuation = 1. / dot(direction_to_light, direction_to_light); // distance squared


    vec3 light_color = ubo.light_color.xyz * ubo.light_color.w * attenuation;
    vec3 ambient_light = ubo.ambient_light_color.xyz * ubo.ambient_light_color.w;

    vec3 diffuse_light = light_color * max(dot(normal_world_space, normalize(direction_to_light)), 0);

    fragColor = color * (diffuse_light + ambient_light);
}