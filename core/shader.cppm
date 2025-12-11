module;
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <print>
#include <sstream>


import core.vertex_buffer;
import core.index_buffer;

export module core.shader;


enum shader_t : int {
    vertex = GL_VERTEX_SHADER,
    fragment = GL_FRAGMENT_SHADER
};

// static int RetrieveShaderType(shader_t ShaderStageType){
//     if(ShaderStageType == shader_t::VERTEX) return GL_VERTEX_SHADER;
//     else if(ShaderStageType == shader_t::FRAGMENT) return GL_FRAGMENT_SHADER;
//     return 0;
// }

static std::string shader_type_to_string(shader_t p_shader_type){
    if(p_shader_type == shader_t::vertex) return "Vertex";
    else if(p_shader_type == shader_t::fragment) return "Fragment";
    
    return "Undefined Shader!";
}

export class shader {
public:
    shader() = default;
    shader(const std::string& p_vertex, const std::string& p_fragment) : m_is_loaded(true), m_path(p_vertex){
        //! @note We initialize our shader program to then take in our shader modules and used for linking them
        m_shader_program_id = glCreateProgram();

        // Load, Build, Compile our vertex and fragment shader modules
        int success;
        bool is_vert_loaded = load_module(p_vertex, shader_t::vertex);

        if(!is_vert_loaded){
            std::print("Vertex Shader DID NOT LOAD CORRECTLY!\n");
            std::print("Could not load filepath = {}\n", p_vertex);
            return;
        }

        bool is_frag_loaded = load_module(p_fragment, shader_t::fragment);
        
        if(!is_frag_loaded){
            std::print("FRAG SHADER did not load correctly!\n");
            std::print("Could not load filepath = {}\n", p_fragment);
            return;
        }

        glLinkProgram(m_shader_program_id);

        glGetProgramiv(m_shader_program_id, GL_LINK_STATUS, &success);
        if(!success){
            char info_log[512];
            glGetProgramInfoLog(m_shader_program_id, 512, nullptr, info_log);
            std::print("Could NOT load shader program!\n");
            std::print("{}\n", info_log);
            return;
        }


    }

    ~shader(){
        glUseProgram(0);
    }

    bool load_module(const std::string& p_filepath, shader_t p_shader_type){
        std::ifstream ins(p_filepath);
        int success;
        std::array<char, 512> info_log;

        if(!ins){
            return false;
        }

        std::stringstream ss;
        ss << ins.rdbuf();
        // This will contain the actual shader literal src
        // const char* shader_src = ss.str().c_str();
        std::string src = ss.str();
        const char* shader_src = src.c_str();

        m_vertex_shader_id = glCreateShader(static_cast<int>(p_shader_type));
        glShaderSource(m_vertex_shader_id, 1, &shader_src, nullptr);
        glCompileShader(m_vertex_shader_id);
        glGetShaderiv(m_vertex_shader_id, GL_COMPILE_STATUS, &success);

        if(!success){
            glGetShaderInfoLog(m_vertex_shader_id, info_log.size(), nullptr, info_log.data());
            std::print("{} SHADER ERROR\n", shader_type_to_string(p_shader_type));
            std::print("{}\n", info_log.data());
            return false;
        }

        glAttachShader(m_shader_program_id, m_vertex_shader_id);
        return true;
    }

    void bind(){
        glUseProgram(m_shader_program_id);
    }

    void unbind(){
        glUseProgram(0);
    }

    void bind(uint32_t p_index=0){
        glUseProgram(p_index);
    }

    void unbind(uint32_t p_index=0){
        glUseProgram(p_index);
    }

    [[nodiscard]] bool loaded() const {
        return m_is_loaded;
    }

    int get_location(const std::string& p_name){
        return glGetUniformLocation(m_shader_program_id, p_name.c_str());
    }

    void set(const std::string& name, const int& p_value){
        glUniform1i(get_location(name), p_value);
    }

    void set(const std::string& name, const float& p_value){
        glUniform1f(get_location(name), p_value);
    }

    void set(const std::string& name, const glm::vec2& p_value){
        glUniform2f(get_location(name), p_value.x, p_value.y);
    }

    void set(const std::string& name, const glm::vec3& p_value){
        glUniform3f(get_location(name), p_value.x, p_value.y, p_value.z);
    }

    void set(const std::string& name, const glm::vec4& p_value){
        glUniform4f(get_location(name), p_value.x, p_value.y, p_value.z, p_value.w);
    }

    void set(const std::string& name, const glm::mat2& p_value){
        glUniformMatrix2fv(get_location(name), 1, false, glm::value_ptr(p_value));
    }

    void set(const std::string& name, const glm::mat3& p_value){
        glUniformMatrix3fv(get_location(name), 1, false, glm::value_ptr(p_value));
    }

    void set(const std::string& name, const glm::mat4& p_value){
        glUniformMatrix4fv(get_location(name), 1, false, glm::value_ptr(p_value));
    }
private:
    uint32_t m_shader_program_id;
    bool m_is_loaded=false;
    std::string m_path;
    uint32_t m_vertex_shader_id;
};