#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 frag_pos_world;
layout(location = 2) in vec3 frag_normal_world;

layout(location = 0) out vec4 outColor;

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
    vec3 direction_to_light = (ubo.light_position - frag_pos_world);
    float attenuation = 1. / dot(direction_to_light, direction_to_light); // distance squared


    vec3 light_color = ubo.light_color.xyz * ubo.light_color.w * attenuation;
    vec3 ambient_light = ubo.ambient_light_color.xyz * ubo.ambient_light_color.w;

    vec3 diffuse_light = light_color * max(dot(normalize(frag_normal_world), normalize(direction_to_light)), 0);

    outColor = vec4((diffuse_light + ambient_light) * frag_color, 1);
}