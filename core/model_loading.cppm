module;

#include <filesystem>
#include <span>

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <print>


import core.vertex_buffer;
import core.index_buffer;
import core.texture;
import core.vertex_array;
import core.shader;

export module core.model_loading;

struct mesh_metadata {
    std::string name="";
    vertex_buffer vertices;
    index_buffer indices;
};


// export class model_loading {
// public:
//     model_loading() = default;

//     model_loading(const std::filesystem::path& p_filepath) {
//     }


//     void invalidate(const std::filesystem::path& p_filepath) {
//         Assimp::Importer importer;
//         const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); 
//     }



// private:
//     void process_node(aiNode* p_node, const aiScene* p_scene) {
//         for(size_t i = 0; i < node->mNumMeshes; i++) {
//             aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//             meshes.emplace_back(process_mesh(mesh, scene));
//         }

//         for(size_t i = 0; i < node->mNumChildren; i++) {
//             processNode(node->mChildren[i], scene);
//         }
//     }


//     void process_each_mesh(aiMesh* p_mesh, const aiScene* p_scene) {
//         mesh_metadata data;
//         std::string name = mesh->mName.C_Str();
//         vertex_buffer vertices;
//         index_buffer indices;

//         // Vertices, Normals, and TexCoords
//         for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            
//             // Position
//             vertex.x = mesh->mVertices[i].x;
//             vertex.y = mesh->mVertices[i].y;
//             vertex.z = mesh->mVertices[i].z;

//             // Normal (Guaranteed to exist by aiProcess_GenSmoothNormals)
//             vertex.normals.x = mesh->mNormals[i].x;
//             vertex.normals.x = mesh->mNormals[i].y;
//             vertex.normals.x = mesh->mNormals[i].z;

//             // Texture Coords (Check if they exist, often 0 or 1 sets)
//             if (mesh->mTextureCoords[0]) {
//                 vertex.uv.x = mesh->mTextureCoords[0][i].x;
//                 vertex.uv.y = mesh->mTextureCoords[0][i].y;
//             }
//             else {
//                 vertex.u = vertex.v = 0.0f;
//             }

//             data.vertices.push_back(vertex);
//         }

//         // Indices (Faces)
//         for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//             aiFace face = mesh->mFaces[i];
//             // Triangulate flag guarantees faces are triangles (3 indices)
//             for (unsigned int j = 0; j < face.mNumIndices; j++) {
//                 data.indices.push_back(face.mIndices[j]);
//             }
//         }

//         m_metadata.emplace_back(name, vertices, indices)
//     }


// private:
//     // std::vector<vertex> m_vertices;
//     // std::vector<uint32_t> m_indices;
//     // std::vector<texture> m_textures;
//     std::vector<mesh_metadata> m_metadata;
//     vertex_array m_vao;
// };
#include <string>
#include <vector>
using namespace std;


struct Vertex{
    static constexpr uint32_t MAX_BONE_INFLUENCE = 4;
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

struct Texture {
    uint32_t m_Temp;
    unsigned int id;
    string type;
    string path;
};

export class mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures){
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void draw(shader &shader) {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;

        for(unsigned int i = 0; i < textures.size(); i++){
            // glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
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
            // and finally bind the texture
            shader.set(std::format("{}\n", (name + number)), int(i));
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
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
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};