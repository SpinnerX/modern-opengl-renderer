import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
import os

class StarterConanRecipe(ConanFile):
    name = "example"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    export_source = "CMakeLists.txt", "Application.cpp"

    # Putting all of your build-related dependencies here
    def build_requirements(self):
        self.tool_requires("cmake/4.1.1")
        self.tool_requires("ninja/1.13.1")
        self.tool_requires("engine3d-cmake-utils/4.0")
        self.tool_requires("cmake-modules-toolchain/1.0.3")

    # Putting all of your packages here
    def requirements(self):
        self.requires("glfw/3.4")
        self.requires("spdlog/1.15.1")
        self.requires("glm/1.0.1")
        self.requires("box2d/2.4.1")
        self.requires("opengl/system")
        self.requires("tinyobjloader/2.0.0-rc10")
        self.requires("assimp/5.4.3")
        self.requires("glad/0.1.36")
        self.requires("flecs/4.0.4")
        self.requires("stb/cci.20230920")
    
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generator = "Ninja"
        tc.cache_variables["CMAKE_CXX_SCAN_FOR_MODULES"] = True
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def layout(self):
        cmake_layout(self)
