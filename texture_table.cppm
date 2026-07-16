
module;

#include <map>
#include <filesystem>
#include <span>



export module core:texture_table;

import :texture;
/*
 *
 * @brief Used to indicate the texture and which texture slot to activate
 *
 * The slot is used because OpenGL requires that you provide a slot to activate this texture with
 * Acts as a generic texture table -- material table
 *
 *
 *
 * Eventually we will use this as a way to handle multiple materials. Where, we may have std::map<entity_id, material_table>
 *
 * This way we can lookup specific materials for those specific 3d models.
 *
 *
 * entity_id is to lookup for specific material mappings
 * Then each entity, contains a slot index that they hold to iterate over the those textures.
 *
 * Example:
 *
 * entity_id: 1000
 *      entity_material_slots: 1, 2, 3, 4
 *
 * We iterate 1 to 4 to get the correct slots for the texture mappings for specifically entity ID 100.
 *
 *
 * NOTE: Should handle that if there is no texture provided, we provide a white texture as default.
 * */
struct texture_slots {
    texture image_texture;
    uint32_t slot=0;
};

export class texture_table {
public:
    texture_table() {}

    void add_slot(uint32_t p_slot, const std::filesystem::path& p_path) {
        m_slots.emplace(p_slot, texture_slots{texture(p_path.string(), false), p_slot});
        /* m_slots[p_slot].image_texture.unbind(); */
    }

    void bind() {
        for(auto&[key, texture_slot] : m_slots) {
            texture_slot.image_texture.bind(key);
        }
    }

    void unbind() {
        for(auto&[key, texture_slot] : m_slots) {
            texture_slot.image_texture.unbind();
        }
    }

    /* void bind_all() { */
    /*     uint32_t slot = 0; */
    /*     for(const auto[key, value] : m_slots) { */
    /*         value.image_texture.bind(slot); */
    /*         slot++; */
    /*     } */
    /* } */

private:
    std::map<uint32_t, texture_slots> m_slots;
};
