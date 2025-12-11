module;

#include <print>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <filesystem>

import core.shader;
import core.vertex_buffer;
import core.index_buffer;
import core.vertex_array;
import core.stb_image;
import core.texture;

export module core.model_tutorial;

struct mesh_metadata {
    uint32_t indices_count=0;
    uint32_t base_vertex=0;
    uint32_t base_index=0;
    uint32_t material_index=0;
};

class model_importer {
public:
    model_importer(const std::filesystem::path& p_filepath) {
        invalidate(p_filepath);
    }

    void invalidate(const std::filesystem::path& p_filepath) {
        m_vao = vertex_array();
        std::vector<vertex_attribute_element> element_attributes = {
            { .name = "aPos", .type = GL_FLOAT, .size = 3},
            { .name = "aColor", .type = GL_FLOAT, .size = 3 },
            { .name = "aNormals", .type = GL_FLOAT, .size = 3},
            { .name = "aTexCoords", .type = GL_FLOAT, .size = 2 },
            { .name = "aTangent", .type = GL_FLOAT, .size = 3 },
            { .name = "aBitTangent", .type = GL_FLOAT, .size = 3 },
        };
        m_vao.vertex_attributes(element_attributes);

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(p_filepath.string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
        if(scene) {
        }
        else {
            std::println("Error parsing {}: {}", p_filepath.string(), importer.GetErrorString());
        }
    }

    void draw(shader& p_shader_source) {
        // for(size_t i = 0; i < m_textures.size(); i++) {
        //     m_textures[i].bind(i);
        // }
    }

private:
    void process_metadata(const aiScene* p_scene, const std::string& p_filename) {
        m_meshes.resize(p_scene->mNumMeshes);
        m_textures.resize(p_scene->mNumMaterials);
        
        uint32_t vertices_count = 0;
        uint32_t indices_count = 0;
        convert_vertices_and_indices(p_scene, vertices_count, indices_count);


    }

    void convert_vertices_and_indices(const aiScene* p_scene, uint32_t& p_vertices_count, uint32_t& p_indices_count) {
        for(size_t i = 0; i < m_meshes.size(); i++){
            const auto& mesh = p_scene->mMeshes[i];
            m_meshes[i].material_index = mesh->mMaterialIndex;
            m_meshes[i].indices_count = mesh->mNumFaces*3;
            m_meshes[i].base_vertex = p_vertices_count;
            m_meshes[i].base_index = p_indices_count;

            p_vertices_count += mesh->mNumVertices;
            p_indices_count += m_meshes[i].indices_count;
        }
    }

private:
    // mesh_metadata m_data;
    vertex_array m_vao;
    // contain all of the mesh individual metadata corresponding to this particular model.
    // using SOA rather then containing AOS
    std::vector<mesh_metadata> m_meshes;
    std::vector<texture> m_textures;
    // std::vector<mesh_metadata> m_data;
};