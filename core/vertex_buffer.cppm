module;

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <span>
#include <print>
#include <cstdint>

//export module vertex_buffer;
export module core.vertex_buffer;


/* struct vertex { */
/*     static constexpr uint32_t max_bone_influence=4; */
/*     glm::vec3 position; */
/*     glm::vec3 color; */
/*     glm::vec3 normal; */
/*     glm::vec2 uv; */
/*     glm::vec3 tangent; */
/*     glm::vec3 bit_tangent; */

/*     bool operator==(const vertex& other) const { */
/*         return position == other.position and color == other.color and */
/*                 uv == other.uv and normal == other.normal; */
/*     } */
/* }; */


struct vertex {
    static constexpr uint32_t max_bone_influence=4;
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 uv;
    // glm::vec3 tangent;
    // glm::vec3 bit_tangent;

    bool operator==(const vertex& other) const {
        return position == other.position and color == other.color and
                uv == other.uv and normal == other.normal;
    }
};

enum class data_type : uint16_t {
    // float1, float2, float3, float4,
    float1,
    mat3, mat4,
    // int1, int2, int3, int4,
    int1,
    bool1
};

export struct vertex_attribute_element {
    std::string name="Undefined";
    GLenum type;
    uint32_t size=0; // size of the components, essentially sizeof(glm::vec3)
    uint32_t offset=0;
    bool is_normalized=false;
};

/*

* in shader representation
layout(location = 0) in vec3 aPos;

* vertex_attribute_element representation
* .size represents the components, in this case glm::vec3 = 3 floats. So size is 3.
* .type represent what is the type of the type each component are, in this case glm::vec3 contains all three floats. Which is why specified as GL_FLOAT
std::array<vertex_attribute_element, 1> attributes = {
    vertex_attribute_element{ .name = "aPos", .type = GL_FLOAT, .size=3 }
};

*/
export class vertex_attribute {
public:
    vertex_attribute() = default;
    vertex_attribute(const std::span<vertex_attribute_element>& p_list){
        calculate_stride_offset(p_list);
    }

    [[nodiscard]] uint32_t stride() const { return m_stride; }

private:
    void calculate_stride_offset(const std::span<vertex_attribute_element>& p_attributes) {
        uint32_t offset = 0;
        m_stride = 0;

        for(auto& attribute : p_attributes) {
            attribute.offset = offset;
            offset += attribute.size;
            m_stride += attribute.size;
        }
    }

private:
    uint32_t m_stride=0;
};


export class vertex_buffer {
public:
    vertex_buffer() = default;
    vertex_buffer(std::span<const vertex> p_vertices) {
        glGenBuffers(1, &m_id);
        write(p_vertices);
    }

    vertex_buffer(std::span<float> p_vertices) {
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

    void write(std::span<float> p_vertices) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, static_cast<uint32_t>(p_vertices.size_bytes()), p_vertices.data(), GL_STATIC_DRAW);
    }

    void write(std::span<const vertex> p_vertices) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, static_cast<uint32_t>(p_vertices.size_bytes()), p_vertices.data(), GL_STATIC_DRAW);
    }

private:
    uint32_t m_id;
};
