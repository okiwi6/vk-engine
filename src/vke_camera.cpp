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

}