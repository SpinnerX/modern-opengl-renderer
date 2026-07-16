module;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <print>
#include <vector>
#include <glm/glm.hpp>

export module core:assimp_loader;

export import :stb_image;
export import :shader;
export import:mesh_batch;
export import :texture;

export namespace core {
    class assimp_loader {
    public:
        assimp_loader(const std::string& p_filename, bool p_enable_gamma, std::span<texture> p_textures) : m_textures(p_textures.begin(), p_textures.end()){
            invalidate(p_filename, p_enable_gamma, p_textures);
        }
        
        void invalidate(const std::string& p_filename, bool p_enable_gamma, std::span<texture> p_textures) {
            // read file via ASSIMP
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(p_filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
            // check for errors
            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
            {
                /* cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl; */
                std::println("ERROR:ASSIMP:: {}", importer.GetErrorString());
                return;
            }
            // retrieve the directory path of the filepath
            m_directory = p_filename.substr(0, p_filename.find_last_of('/'));

            // process ASSIMP's root node recursively
            process_nodes(scene->mRootNode, scene);
        }
        
        void draw(shader &shader, glm::vec3& Position, glm::vec3& Scale, glm::vec3& Rotation, float model_rotation_angle)
        {
            //! @note Apply textures (if enabled)
            /* if(texturesEnabled){ */
            for(uint32_t i = 0; i < m_textures.size(); i++){
                m_textures[i].bind(i);
            }
            /* } */
            for(unsigned int i = 0; i < m_meshes.size(); i++)
                m_meshes[i].draw(Position, Scale, Rotation, model_rotation_angle, shader);

            /* if(texturesEnabled){ */
            for(uint32_t i = 0; i < m_textures.size(); i++){
                m_textures[i].unbind();
            }
            /* } */
        }

        // TODO: Defaults return false, should return true if loaded
        bool loaded() const { return false; }

    private:
            void process_nodes(aiNode* p_root, const aiScene* p_scene) {
                // process each mesh located at the current node
                for(unsigned int i = 0; i < p_root->mNumMeshes; i++)
                {
                    // the node object only contains indices to index the actual objects in the scene. 
                    // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
                    aiMesh* mesh_data = p_scene->mMeshes[p_root->mMeshes[i]];
                    m_meshes.push_back(process_mesh(mesh_data, p_scene));
                }
                // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
                for(unsigned int i = 0; i < p_root->mNumChildren; i++)
                {
                    process_nodes(p_root->mChildren[i], p_scene);
                }
            }

            mesh_batch process_mesh(aiMesh *mesh, const aiScene *scene) {
                // data to fill
                std::vector<vertex_tutorial> vertices;
                std::vector<uint32_t> indices;
                std::vector<texture> textures;

                // walk through each of the mesh's vertices
                for(unsigned int i = 0; i < mesh->mNumVertices; i++)
                {
                    vertex_tutorial vertex;
                    glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
                                      // positions
                    vector.x = mesh->mVertices[i].x;
                    vector.y = mesh->mVertices[i].y;
                    vector.z = mesh->mVertices[i].z;
                    vertex.Position = vector;
                    // normals
                    if (mesh->HasNormals())
                    {
                        vector.x = mesh->mNormals[i].x;
                        vector.y = mesh->mNormals[i].y;
                        vector.z = mesh->mNormals[i].z;
                        vertex.Normal = vector;
                    }
                    // texture coordinates
                    if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
                    {
                        glm::vec2 vec;
                        // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                        // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                        vec.x = mesh->mTextureCoords[0][i].x; 
                        vec.y = mesh->mTextureCoords[0][i].y;
                        vertex.TexCoords = vec;
                        // tangent
                        vector.x = mesh->mTangents[i].x;
                        vector.y = mesh->mTangents[i].y;
                        vector.z = mesh->mTangents[i].z;
                        vertex.Tangent = vector;
                        // bitangent
                        vector.x = mesh->mBitangents[i].x;
                        vector.y = mesh->mBitangents[i].y;
                        vector.z = mesh->mBitangents[i].z;
                        vertex.Bitangent = vector;
                    }
                    else
                        vertex.TexCoords = glm::vec2(0.0f, 0.0f);

                    vertices.push_back(vertex);
                }
                // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
                for(unsigned int i = 0; i < mesh->mNumFaces; i++)
                {
                    aiFace face = mesh->mFaces[i];
                    // retrieve all indices of the face and store them in the indices vector
                    for(unsigned int j = 0; j < face.mNumIndices; j++)
                        indices.push_back(face.mIndices[j]);        
                }
                // process materials
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
                // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
                // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
                // Same applies to other texture as the following list summarizes:
                // diffuse: texture_diffuseN
                // specular: texture_specularN
                // normal: texture_normalN

                // 1. diffuse maps
                std::vector<texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
                // 2. specular maps
                std::vector<texture> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
                // 3. normal maps
                std::vector<texture> normalMaps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
                textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
                // 4. height maps
                std::vector<texture> heightMaps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
                textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

                // return a mesh object created from the extracted mesh data
                return mesh_batch(vertices, indices, textures);
            }
    private:
            std::vector<texture> load_material_textures(aiMaterial *mat, aiTextureType type, std::string typeName) {
                std::vector<texture> textures;
                for(uint32_t i = 0; i < mat->GetTextureCount(type); i++)
                {
                    aiString str;
                    mat->GetTexture(type, i, &str);
                    // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
                    bool skip = false;
                    for(unsigned int j = 0; j < m_textures.size(); j++)
                    {
                        if(std::strcmp(m_textures[j].path().data(), str.C_Str()) == 0)
                        {
                            textures.push_back(m_textures[j]);
                            skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                            break;
                        }
                    }
                    if(!skip)
                    {   // if texture hasn't been loaded already, load it
                        /* Texture texture; */
                        /* texture.id = TextureFromFile(str.C_Str(), this->directory); */
                        /* texture.type = typeName; */
                        /* texture.path = str.C_Str(); */
                        /* textures.push_back(texture); */
                        texture mat_texture(str.C_Str());
                        mat_texture.set_type(typeName);
                        m_textures.push_back(mat_texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
                    }
                }
                return textures;
            }

    private:
        std::string m_directory="";
        std::vector<mesh_batch> m_meshes;
        std::vector<texture> m_textures;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    };
};
