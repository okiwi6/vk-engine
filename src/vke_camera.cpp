#include "vke_camera.hpp"

#include <cassert>
#include <limits>


namespace vke {

void VkeCamera::set_orthographic_projection(float left, float right, float top, float bottom, float near, float far) {
    // defines the orthographic projection to canonical viewing volume
    projection_matrix = glm::mat4{1.0f};
    projection_matrix[0][0] = 2.f / (right - left);
    projection_matrix[1][1] = 2.f / (bottom - top);
    projection_matrix[2][2] = 1.f / (far - near);
    projection_matrix[3][0] = -(right + left) / (right - left);
    projection_matrix[3][1] = -(bottom + top) / (bottom - top);
    projection_matrix[3][2] = -near / (far - near);
}
 
void VkeCamera::set_perspective_projection(float fov_y, float aspect_ratio, float near, float far) {
    // applies frustum perspective projection
    assert(glm::abs(aspect_ratio - std::numeric_limits<float>::epsilon()) > 0.0f);

    const float tan_half_fovy = tan(fov_y / 2.f);
    projection_matrix = glm::mat4{0.0f};
    projection_matrix[0][0] = 1.f / (aspect_ratio * tan_half_fovy);
    projection_matrix[1][1] = 1.f / (tan_half_fovy);
    projection_matrix[2][2] = far / (far - near);
    projection_matrix[2][3] = 1.f;
    projection_matrix[3][2] = -(far * near) / (far - near);
}

void VkeCamera::set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    // ortnormal base
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    view_matrix = glm::mat4{1.f};
    view_matrix[0][0] = u.x;
    view_matrix[1][0] = u.y;
    view_matrix[2][0] = u.z;
    view_matrix[0][1] = v.x;
    view_matrix[1][1] = v.y;
    view_matrix[2][1] = v.z;
    view_matrix[0][2] = w.x;
    view_matrix[1][2] = w.y;
    view_matrix[2][2] = w.z;
    view_matrix[3][0] = -glm::dot(u, position);
    view_matrix[3][1] = -glm::dot(v, position);
    view_matrix[3][2] = -glm::dot(w, position);
}

void VkeCamera::set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    // direction is current to target
    set_view_direction(position, target - position, up);
}

void VkeCamera::set_view_yxz(glm::vec3 position, glm::vec3 rotation) {
  const float c3 = glm::cos(rotation.z);
  const float s3 = glm::sin(rotation.z);
  const float c2 = glm::cos(rotation.x);
  const float s2 = glm::sin(rotation.x);
  const float c1 = glm::cos(rotation.y);
  const float s1 = glm::sin(rotation.y);
  const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
  const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
  const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
  
  view_matrix = glm::mat4{1.f};
  view_matrix[0][0] = u.x;
  view_matrix[1][0] = u.y;
  view_matrix[2][0] = u.z;
  view_matrix[0][1] = v.x;
  view_matrix[1][1] = v.y;
  view_matrix[2][1] = v.z;
  view_matrix[0][2] = w.x;
  view_matrix[1][2] = w.y;
  view_matrix[2][2] = w.z;
  view_matrix[3][0] = -glm::dot(u, position);
  view_matrix[3][1] = -glm::dot(v, position);
  view_matrix[3][2] = -glm::dot(w, position);
}


}