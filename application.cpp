#include <print>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
import core.vertex_buffer;
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
    
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        
        glfwSwapBuffers(window);
    }
    return 0;
}
