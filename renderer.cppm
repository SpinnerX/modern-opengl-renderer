module;

#include <print>
#include <flecs.h>
#include <vector>
#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>

#include <glad/glad.h>


export module core:renderer;


import :components;
import :texture;
import :texture_table;

import :shader;
import :shader_library;
import :obj_model_loader;
import :vertex_buffer;
import :index_buffer;
import :vertex_array;

export class renderer {
public:
    renderer(flecs::world& p_scene) : m_current_scene(&p_scene) {
        m_shader_storage.emplace(shader_type::geometry, "builtin.shaders/geometry.vert", "builtin.shaders/geometry.frag");
        /* m_texture_test = texture("assets/models/wood.png", false); */
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
        auto light_sources = m_current_scene->query_builder<core::transform, core::point_light>();
        uint32_t light_source_count = light_sources.build().count();
        /* geometry_shader->set("num_point_lights", (int)light_source_count); */
        /* struct point_light { */
        /*     glm::vec3 position; // this is provided by the transform */
        /*     glm::vec4 color = { 1.f, 1.f, 1.f, 1.f }; */
        /*     float attenuation = 1.f; */
        /*     float constant = 1.f; */
        /*     float linear = 1.f; */
        /*     float quadratic = 1.f; */

        /*     glm::vec4 ambient = glm::vec4(1.f); */
        /*     glm::vec4 diffuse = glm::vec4(1.f); */
        /*     glm::vec4 specular = glm::vec4(1.f); */
        /* }; */
        uint32_t point_light_index = 0;
        light_sources.each([this, &geometry_shader, &point_light_index](flecs::entity p_entity, core::transform& p_transform, core::point_light& p_light){
            std::string fmt = std::format("point_light[{}]", point_light_index);
            geometry_shader->set(std::format("{}.position", fmt), p_transform.position);
            geometry_shader->set(std::format("{}.color", fmt), p_light.color);
            geometry_shader->set(std::format("{}.attenuation", fmt), p_light.attenuation);
            geometry_shader->set(std::format("{}.constant", fmt), p_light.constant);
            geometry_shader->set(std::format("{}.linear", fmt), p_light.linear);
            geometry_shader->set(std::format("{}.quadratic", fmt), p_light.quadratic);
            geometry_shader->set(std::format("{}.ambient", fmt), p_light.ambient);
            geometry_shader->set(std::format("{}.diffuse", fmt), p_light.diffuse);
            geometry_shader->set(std::format("{}.specular", fmt), p_light.specular);
            point_light_index++;
        });

        /* std::println("num_point_lights = {}", light_source_count); */
        /* std::println("point_light_index = {}", point_light_index); */
        
        sources.each([this](flecs::entity p_entity, core::transform& p_transform, core::mesh_source& p_source){
            if(!m_cached_meshes.contains(p_entity.id())) {
                /* core::obj_model_loader obj_model(p_source.filepath); */
                std::println("entity name = {}", p_entity.name().c_str());
                m_cached_meshes[p_entity.id()] = core::obj_model_loader(p_source.filepath);

                // loading textures
                // 0 = ambient, 1 = diffuse, 2 = specular, 3 = normal
                // if no texture, then replace empty texture with a white texture

                texture_table table;

                // ambient
                if(p_source.ambient.empty()) {
                    std::array<uint8_t, 4> bytes = {0xFF, 0xFF, 0xFF, 0xFF};
                    table.add_slot(0, 1, 1, bytes);
                }
                else {
                    table.add_slot(0, p_source.ambient);
                }

                // add diffuse
                
                if(p_source.diffuse.empty()) {
                    std::array<uint8_t, 4> bytes = {0xFF, 0xFF, 0xFF, 0xFF};
                    table.add_slot(1, 1, 1, bytes);
                }
                else {
                    table.add_slot(1, p_source.diffuse);
                }

                /* // add specular */
                if(p_source.specular.empty()) {
                    std::array<uint8_t, 4> bytes = {0xFF, 0xFF, 0xFF, 0xFF};
                    table.add_slot(2, 1, 1, bytes);
                }
                else {
                    table.add_slot(2, p_source.specular);
                }
                
                // TODO: Add these for mapping out the different materials
                // add normal_map
                // add roughness_map
                // add parallax_mapp

                /* std::string ambient=""; */
                /* std::string diffuse=""; */
                /* std::string specular=""; */
                /* std::string normal_map=""; // normal-mapping texture */
                /* std::string roughness_map=""; // roughness mapping texture */
                /* std::string parallax_mapping=""; // for handling parallax mapping */

                m_material_table.emplace(p_entity.id(), table);
            }

        });

        geometry_shader->bind();

        // activate the texture
        geometry_shader->set("ambient", 0);
        geometry_shader->set("diffuse", 1);

        /* uint8_t bytes_size = 0; */
        sources.each([this, &geometry_shader](flecs::entity p_entity, core::transform& p_transform, core::mesh_source& p_source){
                m_material_table[p_entity.id()].bind(0);
                m_material_table[p_entity.id()].bind(1);

                auto model_test = m_cached_meshes[p_entity.id()];
                glm::mat4 model = glm::mat4(1.f);
                model = glm::translate(model, p_transform.position);
                model = glm::scale(model, p_transform.scale);
                geometry_shader->set("model", model);

                model_test.bind();

                glDrawElements(GL_TRIANGLES, static_cast<int>(model_test.size()), GL_UNSIGNED_INT, nullptr);
        });


        /* glDrawElements(GL_TRIANGLES, static_cast<int>(bytes_size), GL_UNSIGNED_INT, nullptr); */
    }
    

    void end() {
    }

private:
    std::vector<core::vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    flecs::world* m_current_scene;
    /* texture m_texture_test; */
    shader_library m_shader_storage;
    std::map<uint64_t, texture_table> m_material_table;
    std::map<uint64_t, core::obj_model_loader> m_cached_meshes;
};
