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

        void set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = {0.f, -1.f, 0.f});
        void set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0.f, -1.f, 0.f});
        void set_view_yxz(glm::vec3 position, glm::vec3 rotation);

        const glm::mat4& get_projection() const { return projection_matrix; }
        const glm::mat4& get_view() const { return view_matrix; }

        private:
        glm::mat4 projection_matrix{1.f};
        glm::mat4 view_matrix{1.f};
    };
}

#endif