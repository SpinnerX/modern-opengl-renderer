module;

#include <print>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

import core.shader;
import core.vertex_buffer;
import core.index_buffer;
import core.vertex_array;
import core.texture;
#include <filesystem>
#include <vector>

export module core.model_importer;

struct mesh_metadata {
    uint32_t indices_count=0;
    uint32_t base_vertex=0;
    uint32_t base_index=0;
    uint32_t material_index=0;
};


/**
* @brief imports a model
* Handles loading in the vertices/indices of the given model through assimp's importer
* Sets up vertex attributes, storing vertices, indices.
*/
export class model_importer {
public:
    model_importer(const std::string& p_filepath) : m_path(p_filepath){
        invalidate(p_filepath);
    }

    void invalidate(const std::string& p_filepath) {
        // m_vao = vertex_array();
        // std::vector<vertex_attribute_element> element_attributes = {
        //     { .name = "aPos", .type = GL_FLOAT, .size = 3},
        //     { .name = "aColor", .type = GL_FLOAT, .size = 3 },
        //     { .name = "aNormals", .type = GL_FLOAT, .size = 3},
        //     { .name = "aTexCoords", .type = GL_FLOAT, .size = 2 },
        //     { .name = "aTangent", .type = GL_FLOAT, .size = 3 },
        //     { .name = "aBitTangent", .type = GL_FLOAT, .size = 3 },
        // };
        // m_vao.vertex_attributes(element_attributes);

        Assimp::Importer importer;

        std::println("p_filepath.string() = {}", p_filepath);
        const aiScene* scene = importer.ReadFile(p_filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if(!scene or scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE or !scene->mRootNode) {
            std::println("Cannot import to assimp from this path = {}", p_filepath);
            std::println("ERROR::ASSIMP::{}", importer.GetErrorString());
            return;
        }
        else {
            std::println("Loading path = {}", p_filepath);
        }

        if(scene->mRootNode != nullptr) {
            std::println("scene->mRootNode != nullptr");
        }
        else {
            std::println("scene->mRootNode == nullptr");
        }
        process_mesh(scene->mRootNode, scene);

    }

    void draw(shader& p_shader_source) {
        // for(size_t i = 0; i < m_textures.size(); i++) {
        //     m_textures[i].bind(i);
        // }
    }

    [[nodiscard]] std::string_view path() const {
        return m_path;
    }

private:
    void process_mesh(aiNode* p_node, const aiScene* p_scene) {
        std::vector<core::vertex> vertices;
        std::vector<uint32_t> indices;
        // std::vector<Texture> textures;
        std::vector<texture> m_textures;

        // for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        // {
        //     Vertex vertex;
        //     // process vertex positions, normals and texture coordinates
        //     [...]
        //     vertices.push_back(vertex);
        // }
        // // process indices
        // [...]
        // process material
        // if(mesh->mMaterialIndex >= 0)
        // {
        //     [...]
        // }
        std::println("mNumMeshes = {}", p_node->mNumMeshes);
        for(uint64_t i = 0; i < p_node->mNumMeshes; i++) {
            aiMesh* mesh = p_scene->mMeshes[p_node->mMeshes[i]];

            if(mesh == nullptr) {
                std::println("aiMesh* == nullptr!!");
            }
            else {
                std::println("aiMesh* != nullptr!!");
            }


            // process metadata (vertices, indices, textures, etc)

            std::println("mesh->mNumVertices = {}", mesh->mNumVertices);

            // process vertices
            for(uint64_t vertex = 0; vertex < mesh->mNumVertices; vertex++) {
                const auto& position = mesh->mVertices[vertex];
                
                const auto& normals = mesh->mNormals[vertex];

                glm::vec3 pos = glm::vec3(position.x, position.y, position.z);
                glm::vec3 normal(normals.x, normals.y, normals.z);


                glm::vec2 uv(0.f);
                if(mesh->mTextureCoords[0]) {
                    const auto& tex_coord = mesh->mTextureCoords[0][vertex];
                    // uv = {
                    //     .x = tex_coord.x,
                    //     .y = tex_coord.y,
                    // };
                    uv.x = tex_coord.x;
                    uv.y = tex_coord.y;
                }
                core::vertex single_vertex = {
                    .position = pos,
                    .normal = normal,
                    .uv = uv,
                };

                vertices.push_back(single_vertex);
            }


            // process indices

            /*
            for(uint64_t j = 0; j < mesh->mNumFaces; j++) {
                aiFace face = mesh->mFaces[i];
                for(uint64_t k = 0; k < face.mNumIndices; k++) {
                    indices.push_back(face.mIndices[k]);
                }
            }

            // process materials
            // struct Texture {
            //     unsigned int id;
            //     string type;
            //     string path;
            // };


            if(mesh->mMaterialIndex >= 0) {
                aiMaterial* material = p_scene->mMaterials[mesh->mMaterialIndex];

                // loading in diffuse texture mappings
                // std::vector<texture> diffuse;

                // loading diffuse textures
                for(uint64_t j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++) {
                    aiString str;
                    material->GetTexture(aiTextureType_DIFFUSE, j, &str);

                    std::string temp(str.C_Str());

                    std::println("Mateiral Index {}: Path = {}", j, temp);
                }

            }
            */
        }


        for(uint64_t i = 0; i < p_node->mNumChildren; i++) {
            process_mesh(p_node->mChildren[i], p_scene);
        }

        // std::println("Finished Loading in vertices and indices!!");
        // std::println("vertices.size() = {}", vertices.size());
        // std::println("indices.size() = {}", indices.size());
        // m_vao = vertex_array(vertices, indices);
    }

    // void process_node(aiNode* p_root, const aiScene* p_scene) {
    //     for(uint64_t i = 0; i < p_root->mNumChildren; i++) {
    //         process_mesh(p_node->mChildren[i], p_scene);
    //     }
    // }

    // void process_metadata()

private:
    // contain all of the mesh individual metadata corresponding to this particular model.
    // using SOA rather then containing AOS
    // std::vector<mesh_metadata> m_data;

    vertex_array m_vao;
    index_buffer m_ibo;
    std::string m_path="Undefined";;
};