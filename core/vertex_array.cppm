module;

#include <glad/glad.h>
#include <print>
#include <filesystem>
#include <span>

import core.vertex_buffer;
import core.index_buffer;

export module vertex_array;



class vertex_array {
public:
    vertex_array(const std::span<float>& p_vertices, const std::span<uint32_t>& p_indices) {
        glGenVertexArrays(1, &m_id);
        glBindVertexArray(m_id);

    }

    vertex_array(const std::filesystem::path& p_path) {
        glGenVertexArrays(1, &m_id);
        glBindVertexArray(m_id);

    }

    void bind() {
        glBindVertexArray(m_id);
    }

    void unbind() {
        glBindVertexArray(0);
    }

    void vertex_attributes(std::span<vertex_attribute_element> p_attribute_elements) {
        m_attributes = vertex_attribute(p_attribute_elements);
        uint32_t index = 0;
        bind();
        m_vbo.bind();

        for(auto element : p_attribute_elements) {
            /* std::println("Index = {}", index); */
            /* std::println("Attribute size = {}", element.size); */
            /* std::println("Stride = {}", m_attributes.stride()); */
            /* std::println("IsNormalized = {}\n", (element.is_normalized ? "true" : "false")); */
            if(element.type == GL_FLOAT) {
                glVertexAttribPointer(index,
                        (int)element.size,
                        element.type,
                        element.is_normalized ? GL_TRUE : GL_FALSE,
                        int(m_attributes.stride() * sizeof(float)),
                        (const void*)(element.offset * sizeof(float))
                        );
                glEnableVertexAttribArray(index);
                index++;
            }
        }
    }

private:
    uint32_t m_id;
    vertex_buffer m_vbo;
    vertex_attributes m_attributes;
};
