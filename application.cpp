#include <print>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <flecs.h>
#include <glm/glm.hpp>

import core.vertex_buffer;
import core.components;
import core.types;
import core.framebuffer;
import core.shader;
using namespace std;

int main(){
    //! @note Just added the some test code to test the conan-starter setup code
    if(!glfwInit()){
        std::print("glfwInit could not be initialized!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    int width = 800;
    int height = 600;
    std::string title = "Hello Window";
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = (mode->width / 2) - (width / 2);
    height = (mode->height / 2) - (height / 2);
    glfwSetWindowPos(window, (int)width, (int)height);

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::println("Could not load glad!!!");
        return -1;
    }

    // core::window_extent fb_extent = {
    //     .width = width,
    //     .height = height,
    // };
    // core::framebuffer main_framebuffer(fb_extent);

    // setting up scene objects
    auto start_time = std::chrono::high_resolution_clock::now();
    float delta_time = 0.f;

    float aspect_ratio = (float)width / (float)height;

    flecs::world registry;
    flecs::entity e1 = registry.entity("Bob");
    e1.add<core::transform>();

    flecs::entity editor_camera = registry.entity("Editor Camera");
    editor_camera.add<core::transform>();
    editor_camera.add<flecs::pair<core::tags::editor, core::projection_view>>();
    editor_camera.set<core::perspective_camera>({
        .plane = { 0.1f, 1000.f },
        .is_active = true,
        .field_of_view = 45.f,
    });


    auto query_camera_objects = registry.query_builder<flecs::pair<core::tags::editor, core::projection_view>, core::perspective_camera>().build();

    glm::vec4 color = {0.f, 0.5f, 0.5f, 1.f};
   
    
    // loading shaders here
    shader experimental_shader("builtin.shaders/geometry.vert", "builtin.shaders/geometry.frag");



    while(!glfwWindowShouldClose(window)){
        auto current_time = std::chrono::high_resolution_clock::now();
        delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
        start_time = current_time;

        registry.progress(delta_time);

        glClearColor(color.x, color.y, color.z, color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
