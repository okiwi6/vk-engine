#ifndef vke_camera_
    #define vke_camera_

    //libs
    #define GLM_FORCE_RADIANS
    #define GLM_FORCE_DEPTH_ZERO_TO_ONE
    #include <glm/glm.hpp>

namespace vke {
    class VkeCamera {
        public:
        void set_orthographic_projection(float left, float right, float top, float bottom, float near, float far);

        void set_perspective_projection(float fov_y, float aspect_ratio, float near, float far);

        const glm::mat4& get_projection() const { return projection_matrix; }

        private:
        glm::mat4 projection_matrix;
    };
}

#endif