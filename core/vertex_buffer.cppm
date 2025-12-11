module;

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <span>
#include <print>

//export module vertex_buffer;
export module core.vertex_buffer;


struct vertex {
    static constexpr uint32_t max_bone_influence=4;
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bit_tangent;

    bool operator==(const vertex& other) const {
        return position == other.position and color == other.color and
                uv == other.uv and normal == other.normal;
    }
};


export void hello() {
    std::println("Hello!");
}

export namespace atlas {
    inline namespace v1 {
        void hello1() {
            std::println("Hi");
        }
    };

    inline namespace v2 {
    };

};


class vertex_buffer {
public:
    vertex_buffer(const std::span<const vertex>& p_vertices) {
        glGenBuffers(1, &m_id);
        write(p_vertices);
    }

    vertex_buffer(const std::span<float>& p_vertices) {
        glGenBuffers(1, &m_id);
        write(p_vertices);
    }

    ~vertex_buffer() {
        unbind();
    }

    void bind() {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    }

    void unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void write(const std::span<float>& p_vertices) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, static_cast<uint32_t>(p_vertices.size_bytes()), p_vertices.data(), GL_STATIC_DRAW);
    }

    void write(const std::span<const vertex>& p_vertices) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, static_cast<uint32_t>(p_vertices.size_bytes()), p_vertices.data(), GL_STATIC_DRAW);
    }

private:
    uint32_t m_id;
};
