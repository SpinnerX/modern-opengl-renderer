cmake_minimum_required(VERSION 4.0)
project(Starter CXX)



set(CMAKE_CXX_STANDARD 23)



build_application(
    SOURCES
    application.cpp

    PACKAGES
    glfw3
    fmt
    assimp
    glad
    stb
    flecs
    tinyobjloader

    LINK_PACKAGES
    glfw
    fmt::fmt
    assimp::assimp
    glad::glad
    stb::stb
    flecs::flecs_static
    tinyobjloader::tinyobjloader
)

generate_compile_commands()

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD d0edc3af-4c50-42ea-a356-e2862fe7a444)
target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
    renderer.cppm
    core/core.cppm
    core/model_importer.cppm
    core/obj_model_loader.cppm
    core/vertex_buffer.cppm
    core/index_buffer.cppm
    core/vertex_array.cppm
    core/texture.cppm
    core/stb_image.cppm
    core/shader.cppm
    core/components.cppm
    core/types.cppm
    core/framebuffer.cppm
    core/event.cppm
    core/keys.cppm
    core/utilities.cppm
    core/assimp_loader.cppm
    core/mesh_batch.cppm
    core/shader_library.cppm
    texture_table.cppm
)


# target_compile_options(${PROJECT_NAME} PUBLIC -ftime-report)

# target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
