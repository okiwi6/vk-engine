#ifndef vke_game_object_
    #define vke_game_object_

    #include "vke_model.hpp"
    #include <glm/gtc/matrix_transform.hpp>

    //std
    #include <memory>

    namespace vke {

        struct TransformComponent {
            glm::vec3 translation{}; // translation of object
            glm::vec3 scale{1.0f, 1.0f, 1.0f};
            glm::vec3 rotation{};

            glm::mat4 mat4() {
                // Identity * Translate
                auto transform = glm::translate(glm::mat4{1.f}, translation);

                // * Rotation (Y1, X2, Z3)
                transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f});
                transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f});
                transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f});
                
                // * Scale
                transform = glm::scale(transform, scale);

                return transform;
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
            TransformComponent transform{};

            private:
            VkeGameObject(id_t obj_id) : id(obj_id) {}

            id_t id;
        };
    }
    
#endif
