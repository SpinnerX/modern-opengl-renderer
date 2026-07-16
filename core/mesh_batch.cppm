module;

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <span>
#include <glad/glad.h>
#include <print>
#include <format>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
export module core:mesh_batch;

export import :texture;
export import :shader;


export namespace core {
    const int MAX_BONE_INFLUENCE = 4;
    struct vertex_tutorial {
        // position
        glm::vec3 Position;
        // normal
        glm::vec3 Normal;
        // texCoords
        glm::vec2 TexCoords;
        // tangent
        glm::vec3 Tangent;
        // bitangent
        glm::vec3 Bitangent;
        //bone indexes which will influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        //weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE];
    };

    class mesh_batch {
    public:
        mesh_batch(std::span<vertex_tutorial> p_vertices, std::span<uint32_t> p_indices, std::span<texture> p_textures) : m_vertices(p_vertices.begin(), p_vertices.end()), m_indices(p_indices.begin(), p_indices.end()), m_textures(p_textures.begin(), p_textures.end()) {}

        // sets up the mesh
        void invalidate(std::span<vertex_tutorial> p_vertices, std::span<uint8_t> p_indices) { 
            // create buffers/arrays
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            // load data into vertex buffers
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // A great thing about structs is that their memory layout is sequential for all its items.
            // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
            // again translates to 3/2 floats which translates to a byte array.
            std::span<vertex_tutorial> vertices(m_vertices);
            std::span<uint32_t> indices(m_indices);
            glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);  

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);

            // set the vertex attribute pointers
            // vertex Positions
            glEnableVertexAttribArray(0);	
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_tutorial), (void*)0);
            // vertex normals
            glEnableVertexAttribArray(1);	
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_tutorial), (void*)offsetof(vertex_tutorial, Normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);	
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_tutorial), (void*)offsetof(vertex_tutorial, TexCoords));
            // vertex tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_tutorial), (void*)offsetof(vertex_tutorial, Tangent));
            // vertex bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_tutorial), (void*)offsetof(vertex_tutorial, Bitangent));
            // ids
            glEnableVertexAttribArray(5);
            glVertexAttribIPointer(5, 4, GL_INT, sizeof(vertex_tutorial), (void*)offsetof(vertex_tutorial, m_BoneIDs));

            // weights
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_tutorial), (void*)offsetof(vertex_tutorial, m_Weights));
            glBindVertexArray(0);
        }


        /* void draw(glm::vec3 position, shader& p_shader) { */
        void draw(glm::vec3& position, glm::vec3& scale, glm::vec3& rotation, float rotation_angle, shader& p_shader) {
            glm::mat4 model = glm::mat4(1.0f);
            p_shader.set("model", model);
            // bind appropriate textures
            unsigned int diffuseNr  = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr   = 1;
            unsigned int heightNr   = 1;
            for(unsigned int i = 0; i < m_textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                                                  // retrieve texture number (the N in diffuse_textureN)
                std::string number;
                std::string name = m_textures[i].get_type();
                if(name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if(name == "texture_specular")
                    number = std::to_string(specularNr++); // transfer unsigned int to string
                else if(name == "texture_normal")
                    number = std::to_string(normalNr++); // transfer unsigned int to string
                else if(name == "texture_height")
                    number = std::to_string(heightNr++); // transfer unsigned int to string

                // now set the sampler to the correct texture unit
                // glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
                // // and finally bind the texture
                p_shader.set(std::format("{}", (name + number)), int(i));
            }
            model = glm::translate(model, position);
            model = glm::scale(model, scale);
            p_shader.set("model", model);

            // draw mesh
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // always good practice to set everything back to defaults once configured.
            glActiveTexture(GL_TEXTURE0);
        }

    private:
        std::vector<vertex_tutorial> m_vertices;
        std::vector<uint32_t> m_indices;
        std::vector<texture> m_textures;
        uint32_t VBO, EBO, VAO;
    };
};
