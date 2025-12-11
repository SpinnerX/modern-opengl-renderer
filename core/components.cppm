module;

#include <cstdint>
#include <glm/glm.hpp>
#include <filesystem>
export module core.components;


export namespace core {
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
