module;

#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <stb_image.h>
#include <print>
#include <span>



export module core:texture;

import :stb_image;

//! @note Specifying what textures to load the texture 2d in.
enum class texture_type{
    depth_map,
    normal_map,
    gamma_correction
};

export class texture {
public:
    texture() = default;
    
    texture(const std::string& p_filename, bool enable_gamma_correction=false) : m_path(p_filename) {
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
    
        if(p_filename.empty()) {
            std::println("path is empty!!!");
            std::array<uint8_t, 4> bytes = {0xFF, 0xFF, 0xFF, 0xFF};
            create(1, 1, bytes);
        }
        else {
           create(p_filename, enable_gamma_correction); 
        }
        std::println("===============>>>> creating texture with path = {}", m_path);
    }

    void create(const std::string& p_filename, bool enable_gamma_correction) {
        /* // load and generate the texture */
        /* int image_width, image_height, pixel_channels; */
        /* stbi_set_flip_vertically_on_load(true); */
        /* unsigned char *data = stbi_load(p_filename.c_str(), &image_width, &image_height, &pixel_channels, 0); */

        /* std::print("Pixel Channels = {}\n", pixel_channels); */
        /* int internal_format; */
        /* int format_to_use; */

        /* if(pixel_channels == 1){ */
        /*     internal_format = GL_RED; */
        /*     format_to_use = GL_RED; */
        /* } */
        /* else if(pixel_channels == 3){ */
        /*     internal_format = GL_RGB; */
        /*     format_to_use = GL_RGB; */
        /* } */
        /* else if(pixel_channels == 4){ */
        /*     internal_format = GL_RGBA; */
        /*     format_to_use = GL_RGBA; */
        /* } */



        /* if (data){ */
        /*     if(!enable_gamma_correction){ */
        /*         std::println("Not enabling gamma correction!"); */
        /*         glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_width, image_height, 0, format_to_use, GL_UNSIGNED_BYTE, data); */
        /*     } */
        /*     else{ */
        /*         internal_format = GL_SRGB; */
        /*         format_to_use = GL_RGB; */
        /*         glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_width, image_height, 0, format_to_use, GL_UNSIGNED_BYTE, data); */
        /*     } */
        /*     glGenerateMipmap(GL_TEXTURE_2D); */

        /*     // set the texture wrapping/filtering options (on the currently bound texture object) */
        /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); */	
        /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); */
        /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); */
        /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); */
        /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); */
        /*     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
        /* } */
        /* else */
        /* { */
        /*     // std::cout << "Failed to load texture" << std::endl; */
        /*     std::print("Failed to load texture!!\n"); */
        /* } */
        /* stbi_image_free(data); */

        int image_width, image_height, pixel_channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(p_filename.c_str(), &image_width, &image_height, &pixel_channels, 0);

        if (data) {
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

            if(!enable_gamma_correction){
                std::println("Not enabling gamma correction!");
                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_width, image_height, 0, format_to_use, GL_UNSIGNED_BYTE, data);
            }
            else{
                internal_format = GL_SRGB;
                format_to_use = GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image_width, image_height, 0, format_to_use, GL_UNSIGNED_BYTE, data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);

            // set the texture wrapping/filtering options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
    }

    void create(uint32_t p_width, uint32_t p_height, std::span<const uint8_t> p_bytes) {
		m_width = p_width;
        m_height = p_height;
        // Uploading data to OpenGL texture
		// internalData is our internalFormat
        m_internal_format = GL_RGBA8;
		m_format = GL_RGBA;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		// Since we segfault using glTextureParameteri, we'll use this for now
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        uint32_t bpp = m_format == GL_RGBA ? 4 : 3;

        /* assert((size == width * height * bpp)); */
        glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, p_width, p_height, 0, m_format, GL_UNSIGNED_BYTE, p_bytes.data()); // same thing as doing: glTextureSubImage2D
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void bind(int p_texture_slot=0){
        if(m_id == 0) {
            std::println("Tried to bind an invalid texture!!");
            std::println("Texture with this specified path = {}", m_path);
            GLenum error = glGetError();
            std::println("Error = {}", error);
            return;
        }

        glActiveTexture(GL_TEXTURE0 + p_texture_slot);
        /* glActiveTexture(GL_TEXTURE0); */
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void unbind(){
        glActiveTexture(GL_TEXTURE0);
        /* glDisable(GL_TEXTURE_2D); */
    }

    void set_type(const std::string& p_type) {
        m_type = p_type;
    }

    std::string get_type() const { return m_type; }

    [[nodiscard]] std::string_view path() const {
        return m_path;
    }

private:
    uint32_t m_id=0;
    std::string m_path="Undeifned";
    GLenum m_internal_format;
    GLenum m_format;
    uint32_t m_width=0;
    uint32_t m_height=0;
    std::string m_type="";
};
