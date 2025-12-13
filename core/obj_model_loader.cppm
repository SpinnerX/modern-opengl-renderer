module;

#include <filesystem>

#include <tiny_obj_loader.h>
#include <unordered_map>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <print>
#include <glad/glad.h>

import core.vertex_array;
import core.vertex_buffer;
import core.index_buffer;

export module core.obj_model_loader;

namespace core {
    template<typename T, typename... Rest>
    void hash_combine(size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed << 2);
        (hash_combine(seed, rest), ...);
    }

};

namespace std {

    template<>
    struct hash<core::vertex> {
        size_t operator()(const core::vertex& vertex) const {
            size_t seed = 0;
            core::hash_combine(
              seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };

}


export namespace core {

    class obj_model_loader {
    public:
        obj_model_loader(const std::filesystem::path& p_filepath, bool p_flip=false) {
            invalidate(p_filepath);
        }

        void invalidate(const std::filesystem::path& p_filepath, bool p_flip=false) {
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;

            //! @note If we return the constructor then we can check if the mesh
            //! loaded successfully
            //! @note We also receive hints if the loading is successful!
            //! @note Return default constructor automatically returns false means
            //! that mesh will return the boolean as false because it wasnt
            //! successful
            if (!tinyobj::LoadObj(&attrib,
                                &shapes,
                                &materials,
                                &warn,
                                &err,
                                p_filepath.string().c_str())) {
                std::println("Could not load model from path {}", p_filepath.string());
                // m_model_loaded = false;
                return;
            }

            std::vector<core::vertex> vertices;
            std::vector<uint32_t> indices;
            std::unordered_map<core::vertex, uint32_t> unique_vertices{};

            // for (const auto& shape : shapes) {
            for (size_t i = 0; i < shapes.size(); i++) {
                auto shape = shapes[i];
                // for (const auto& index : shape.mesh.indices) {
                for (size_t j = 0; j < shape.mesh.indices.size(); j++) {
                    auto index = shape.mesh.indices[j];
                    core::vertex single_vertex{};

                    if (!unique_vertices.contains(single_vertex)) {
                        unique_vertices[single_vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(single_vertex);
                    }

                    if (index.vertex_index >= 0) {
                        single_vertex.position = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]
                        };

                        single_vertex.color = {
                            attrib.colors[3 * index.vertex_index + 0],
                            attrib.colors[3 * index.vertex_index + 1],
                            attrib.colors[3 * index.vertex_index + 2]
                        };
                    }

                    if (!attrib.normals.empty()) {
                        single_vertex.normal = {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]
                        };
                    }

                    glm::vec3 uv;
                    if (!attrib.texcoords.empty()) {
                        glm::vec2 flipped_uv;
                        flipped_uv = {
                            attrib.texcoords[static_cast<long long>(index.texcoord_index) * 2],
                            1.0f - attrib.texcoords[static_cast<long long>(index.texcoord_index) *2 +1],
                        };

                        glm::vec2 original_uv;
                        
                        original_uv = {
                            attrib.texcoords
                            [static_cast<long long>(index.texcoord_index) * 2],
                            attrib.texcoords
                            [static_cast<long long>(index.texcoord_index) * 2 +
                            1],
                        };

                        single_vertex.uv = p_flip ? flipped_uv : original_uv;
                    }
                    else {
                        single_vertex.uv = glm::vec2(0.f, 0.f);
                    }

                    if (!unique_vertices.contains(single_vertex)) {
                        unique_vertices[single_vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(single_vertex);
                    }

                    indices.push_back(unique_vertices[single_vertex]);
                }
            }

            m_vao = vertex_array(vertices, indices);
            std::array<vertex_attribute_element, 4> elements = {
                vertex_attribute_element{ .name = "aPos", .type = GL_FLOAT, .size = 3, },
                vertex_attribute_element{ .name = "aColor", .type = GL_FLOAT, .size = 3, },
                vertex_attribute_element{ .name = "aNormals", .type = GL_FLOAT, .size = 3, },
                vertex_attribute_element{ .name = "aTexCoords", .type = GL_FLOAT, .size = 2, }
            };

            m_vao.vertex_attributes(elements);
            m_indices_count = indices.size();
        }

        void bind() {
            m_vao.bind();
        }

        [[nodiscard]] uint64_t size() const {
            return m_indices_count;
        }

    private:
        vertex_array m_vao;
        uint64_t m_indices_count=0;
    };
};