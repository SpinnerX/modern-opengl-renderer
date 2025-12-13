#include <print>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <flecs.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

import core.vertex_buffer;
import core.components;
import core.types;
import core.framebuffer;
import core.shader;
import core.vertex_array;
import core.event;
import core.keys;
import core.model_importer;
import core.obj_model_loader;
import core.event;
import core.keys;
import core.utilities;

#include <vector>
#include <filesystem>

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
    registry.system<flecs::pair<core::tags::editor, core::projection_view>, core::transform, core::perspective_camera>().each([aspect_ratio](flecs::pair<core::tags::editor, core::projection_view> p_pair, core::transform p_transform, const core::perspective_camera& p_camera){
        if(!p_camera.is_active) {
            return;
        }

        p_pair->projection = glm::mat4(1.f);

        p_pair->projection =
        glm::perspective(glm::radians(p_camera.field_of_view),
                            aspect_ratio,
                            p_camera.plane.x,
                            p_camera.plane.y);
        p_pair->view = glm::mat4(1.f);
        glm::quat quaternion = to_quat(p_transform.quaternion);
        p_pair->view =
        glm::translate(p_pair->view, p_transform.position) *
        glm::mat4_cast(quaternion);

        p_pair->view = glm::inverse(p_pair->view);
    });

    flecs::entity e1 = registry.entity("Bob");
    e1.add<core::transform>();

    flecs::entity editor_camera = registry.entity("Editor Camera");
    editor_camera.set<core::transform>({
        .position = {0.f, 0.f, 25.90f}
    });
    editor_camera.add<flecs::pair<core::tags::editor, core::projection_view>>();
    editor_camera.set<core::perspective_camera>({
        .plane = { 0.1f, 1000.f },
        .is_active = true,
        .field_of_view = 45.f,
    });

    flecs::entity backpack = registry.entity("Backpack");
    backpack.set<core::transform>({
    });

    glm::vec4 color = {0.f, 0.5f, 0.5f, 1.f};
   
    
    // loading shaders here
    shader experimental_shader("builtin.shaders/geometry.vert", "builtin.shaders/geometry.frag");
    glm::mat4 proj_view(1.f);


    // model_importer import_mesh(std::filesystem::path("backpack/backpack.obj"));
    // std::println("Finished loading path = {}", import_mesh.path());

    core::obj_model_loader obj_model(std::filesystem::path("assets/models/robot.obj"));

    // query all camera objects
    auto query_camera_objects = registry.query_builder<flecs::pair<core::tags::editor, core::projection_view>, core::perspective_camera, core::transform>().build();

    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window)){
        auto current_time = std::chrono::high_resolution_clock::now();
        delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
        start_time = current_time;

        registry.progress(delta_time);

        core::transform* camera_transform = editor_camera.get_mut<core::transform>();

        float movement_speed = 10.f;
        float rotation_speed = 1.f;
        float velocity = movement_speed * delta_time;
        float rotation_velocity = rotation_speed * delta_time;
        glm::quat to_quaternion = to_quat(camera_transform->quaternion);
        glm::vec3 up = glm::rotate(to_quaternion, glm::vec3(0.f, 1.f, 0.f));
          glm::vec3 forward =
            glm::rotate(to_quaternion, glm::vec3(0.f, 0.f, -1.f));
          glm::vec3 right =
            glm::rotate(to_quaternion, glm::vec3(1.0f, 0.0f, 0.0f));

        if (core::is_key_pressed(window, key_w)) {
            camera_transform->position += forward * velocity;
        }
        if (core::is_key_pressed(window, key_s)) {
            camera_transform->position -= forward * velocity;
        }

        if (core::is_key_pressed(window, key_d)) {
            camera_transform->position += right * velocity;
        }
        if (core::is_key_pressed(window, key_a)) {
            camera_transform->position -= right * velocity;
        }
        if (core::is_key_pressed(window, key_q)) {
            camera_transform->rotation.y += rotation_velocity;
        }
        if (core::is_key_pressed(window, key_e)) {
            camera_transform->rotation.y -= rotation_velocity;
        }

        if(core::is_key_pressed(window, key_left_shift)) {
            camera_transform->position += up * velocity;
        }

        if(core::is_key_pressed(window, key_space)) {
            camera_transform->position -= up * velocity;
        }

        query_camera_objects.each(
              [&](flecs::entity,
                flecs::pair<core::tags::editor, core::projection_view> p_pair,
                core::perspective_camera& p_camera, core::transform& p_transform) {
            if (!p_camera.is_active) {
                return;
            }

            proj_view = p_pair->projection * p_pair->view;
        });


        glClearColor(color.x, color.y, color.z, color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        experimental_shader.bind();
        experimental_shader.set("proj_view", proj_view);
        experimental_shader.unbind();

        experimental_shader.bind();
        glm::mat4 model = glm::mat4(1.f);
        const core::transform* t = backpack.get<core::transform>();
        model = glm::translate(model, t->position);
        model = glm::scale(model, t->scale);
        experimental_shader.set("model", model);

        obj_model.bind();
        glDrawElements(GL_TRIANGLES, static_cast<int>(obj_model.size()), GL_UNSIGNED_INT, nullptr);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
