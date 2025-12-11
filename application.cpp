#include <print>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <flecs.h>
#include <glm/glm.hpp>

import core.vertex_buffer;
import core.components;
using namespace std;


int main(){
    //! @note Just added the some test code to test the conan-starter setup code
    if(!glfwInit()){
        std::print("glfwInit could not be initialized!\n");
        return -1;
    }
    
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

    // setting up scene objects
    auto start_time = std::chrono::high_resolution_clock::now();
    float delta_time = 0.f;

    float aspect_ratio = (float)width / (float)height;

    flecs::world registry;
    flecs::entity e1 = registry.entity("Bob");
    e1.add<core::transform>();
    
    flecs::entity e2 = registry.entity("Alice");
    e1.add<core::transform>();

    
    while(!glfwWindowShouldClose(window)){
        auto current_time = std::chrono::high_resolution_clock::now();
        delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
        start_time = current_time;
        glfwPollEvents();
        
        glfwSwapBuffers(window);
    }
    return 0;
}
