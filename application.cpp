#include <print>
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
    
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        
        glfwSwapBuffers(window);

    }
    return 0;
}
