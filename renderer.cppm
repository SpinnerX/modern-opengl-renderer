module;

#include <print>
#include <flecs.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>

#include <glad/glad.h>

#include <optional>
#include <map>

export module core.renderer;

import core.shader;
import core.shader_library;
import core.components;
import core.obj_model_loader;

export class renderer {
public:
    renderer(flecs::world& p_scene) : m_current_scene(&p_scene) {
        m_shader_storage.emplace(shader_type::geometry, "builtin.shaders/geometry.vert", "builtin.shaders/geometry.frag");
    }

    void background_color(const glm::vec4& p_color) {
    }

    void current_scene(flecs::world& p_scene) {
        m_current_scene = &p_scene;
    }

    void begin(glm::mat4 p_proj_view) {

        // we bind geometry shader
        // set the proj_view uniform over the shader
        std::optional<shader> geometry_shader = m_shader_storage.get(shader_type::geometry);
        geometry_shader->bind();
        geometry_shader->set("proj_view", p_proj_view);
    
        auto sources = m_current_scene->query_builder<core::transform, core::mesh_source>().build();

        
        sources.each([this](flecs::entity p_entity, core::transform& p_transform, core::mesh_source& p_source){
            if(!m_cached_meshes.contains(p_entity.id())) {
                /* core::obj_model_loader obj_model(p_source.filepath); */
                std::println("entity name = {}", p_entity.name().c_str());
                m_cached_meshes[p_entity.id()] = core::obj_model_loader(p_source.filepath);
            }

        });

        geometry_shader->bind();
        sources.each([this, &geometry_shader](flecs::entity p_entity, core::transform& p_transform, core::mesh_source& p_source){
                auto model_test = m_cached_meshes[p_entity.id()];
                glm::mat4 model = glm::mat4(1.f);
                model = glm::translate(model, p_transform.position);
                model = glm::scale(model, p_transform.scale);
                geometry_shader->set("model", model);

                model_test.bind();
                glDrawElements(GL_TRIANGLES, static_cast<int>(model_test.size()), GL_UNSIGNED_INT, nullptr);
        });

    }
    

    void end() {
    }

private:
    flecs::world* m_current_scene;
    shader_library m_shader_storage;
    std::map<uint64_t, core::obj_model_loader> m_cached_meshes;
};
