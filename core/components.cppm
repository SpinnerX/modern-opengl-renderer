module;

#include <cstdint>
#include <glm/glm.hpp>
#include <filesystem>
export module core.components;


export namespace core {
    struct perspective_camera {
        // glm::vec2 represented as {near: x, far: y}
        glm::vec2 plane{ 0.f };

        // Sets camera to be the main camera
        bool is_active = false;

        // Specify camera field of view
        // Defaults to 45.0f in radians
        float field_of_view = glm::radians(45.f);
    };


    /**
    * @brief because every object contains a transform component
    * 
    * This component will assume that is already handled and will only contain properties for configuring the point light itself.
    */
    struct point_light {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec4 color{1.f};
    };

    struct projection_view {
        glm::mat4 projection;
        glm::mat4 view;
    };

    namespace tags {
        struct editor {};
    };

    struct transform {
        glm::vec3 position{0.f};
        glm::vec3 scale{0.f};
        glm::vec3 rotation{0.f};
    };

    struct mesh_source {
        std::filesystem::path filepath;
        bool flip_texcoords=false;
    };
};
