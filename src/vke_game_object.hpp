#ifndef vke_game_object_
    #define vke_game_object_

    #include "vke_model.hpp"

    //std
    #include <memory>

    namespace vke {

        struct Transform2dComponent {
            glm::vec2 translation{}; // translation of object
            glm::vec2 scale{1.0f, 1.0f};
            float rotation;

            glm::mat2 mat2() {
                const float s = glm::sin(rotation);
                const float c = glm::cos(rotation);
                
                // Careful! Matrix initizialized column-wise
                glm::mat2 rot_matrix{{c, s}, {-s, c}};       
                glm::mat2 scale_mat{{scale.x, 0.0f}, {0.0f, scale.y}};
                return  rot_matrix * scale_mat;
            };
        };

        class VkeGameObject {
            public:
            using id_t = unsigned int;

            static VkeGameObject create_game_object() {
                static id_t current_id = 0;
                // ids unique and increasing
                return VkeGameObject{current_id++};
            }

            VkeGameObject(const VkeGameObject&) = delete;
            VkeGameObject& operator=(const VkeGameObject&) = delete;
            VkeGameObject(VkeGameObject&&) = default;
            VkeGameObject &operator=(VkeGameObject&&) = default;

            id_t get_id() { return id; }

            std::shared_ptr<VkeModel> model{};
            glm::vec3 color{};
            Transform2dComponent transform2d{};

            private:
            VkeGameObject(id_t obj_id) : id(obj_id) {}

            id_t id;
        };
    }
    
#endif
