module;

#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <stb_image.h>
#include <print>
import core.stb_image;

export module core.texture;


//! @note Specifying what textures to load the texture 2d in.
enum class texture_type{
    depth_map,
    normal_map,
    gamma_correction
};

export class texture {
public:
    texture() = default;
    texture(const std::string& p_filename, bool enable_gamma_correction) : m_path(p_filename) {
        glGenTextures(1, &m_id);
        bind();

        // load and generate the texture
        int image_width, image_height, pixel_channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(p_filename.c_str(), &image_width, &image_height, &pixel_channels, 0);

        std::print("Pixel Channels = {}\n", pixel_channels);
        int internal_format;
        int format_to_use;

        if(pixel_channels == 1){
            internal_format = GL_RED;
            format_to_use = GL_RED;
        }
        else if(pixel_channels == 3){
            internal_format = GL_RGB;
            format_to_use = GL_RGB;
        }
        else if(pixel_channels == 4){
            internal_format = GL_RGBA;
            format_to_use = GL_RGBA;
        }



        if (data){
            if(!enable_gamma_correction){
                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_width, image_height, 0, format_to_use, GL_UNSIGNED_BYTE, data);
            }
            else{
                internal_format = GL_SRGB;
                format_to_use = GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_width, image_height, 0, format_to_use, GL_UNSIGNED_BYTE, data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);

            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            // std::cout << "Failed to load texture" << std::endl;
            std::print("Failed to load texture!!\n");
        }
        stbi_image_free(data);


    }

    texture(const std::string& p_filename, texture_type property_on_load){
        create(p_filename, property_on_load);
    }


    texture(int p_width, int p_height, bool attach_depth_stencil){
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        if(attach_depth_stencil){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, p_width, p_height, 0, 
                    GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        }
        else{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p_width, p_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void create(const std::string& p_filename, texture_type property_on_load){
        glGenTextures(1, &m_id);
        bind();

        // load and generate the texture
        int image_width, image_height, pixel_channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(p_filename.c_str(), &image_width, &image_height, &pixel_channels, 0);

        std::print("Pixel Channels = {}\n", pixel_channels);
        GLenum internal_format;
        GLenum format_to_use;

        if(pixel_channels == 1){
            internal_format = GL_RED;
            format_to_use = GL_RED;
        }
        else if(pixel_channels == 3){
            internal_format = GL_RGB;
            format_to_use = GL_RGB;
        }
        else if(pixel_channels == 4){
            internal_format = GL_RGBA;
            format_to_use = GL_RGBA;
        }


        //! @note TODO: Handle texture to indicate what kinds of textures we are loading
        //! @note GAMMA_CORRECTION means we are enabling gamma correction for the specific texture
        if (data){
            if(property_on_load == texture_type::gamma_correction){
                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_width, image_height, 0, format_to_use, GL_UNSIGNED_BYTE, data);
            }
            else{
                internal_format = GL_SRGB;
                format_to_use = GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_width, image_height, 0, format_to_use, GL_UNSIGNED_BYTE, data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);

            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            // std::cout << "Failed to load texture" << std::endl;
            std::print("Failed to load texture!!\n");
        }
        stbi_image_free(data);
    }

    void bind(int p_texture_slot=0){
        if(p_texture_slot == 0){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_id);
            return;
        }

        glActiveTexture(GL_TEXTURE0 + p_texture_slot);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void unbind(){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        // glDisable(GL_TEXTURE_2D);
    }

    [[nodiscard]] std::string_view path() const {
        return m_path;
    }

private:
    uint32_t m_id;
    std::string m_path="Undeifned";
};
