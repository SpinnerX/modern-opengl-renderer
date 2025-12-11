module;

#include <span>
#include <glad/glad.h>


export module index_buffer;


class index_buffer {
public:
    
    index_buffer(const std::span<const uint32_t>& p_indices) {
        glGenBuffers(1, &m_id);
        invalidate(p_indices);
    }

    void bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    }

    void unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void invalidate(const std::span<const uint32_t>& p_indices) {
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int)p_indices.size_bytes(), p_indices.data(), GL_STATIC_DRAW);
    }
private:
    uint32_t m_id;
};
