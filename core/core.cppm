
export module core;

export import :types;
export import :utilities;
export import :components;
export import :framebuffer;
export import :obj_model_loader;
export import :vertex_buffer;
export import :index_buffer;
export import :vertex_array;
/* export import :model_loader; */
export import :obj_model_loader;
export import :shader;
export import :renderer;
export import :shader_library;

export import :event;
export import :keys;

export import :assimp_loader;
export import :mesh_batch;

export {
#ifdef TEST_MACRO
void print() {
    std::println("Macro Defined!");
}
#endif
}


namespace core {
};
