module;

#include <unordered_map>


#include <optional>

export module core:shader_library;


import :shader;

export enum class shader_type {
    geometry,
    compute,
};

export class shader_library {
public:
    shader_library() = default;

    std::optional<shader> get(shader_type p_type) {
        return m_shaders[p_type];
    }
    
    void emplace(shader_type p_type, const std::string& p_vert, const std::string& p_frag) {
        m_shaders.insert({p_type, shader(p_vert, p_frag)});
    }

private:
    std::unordered_map<shader_type, shader> m_shaders;
};
