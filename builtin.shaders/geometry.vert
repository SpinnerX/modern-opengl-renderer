#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormals;
layout(location = 3) in vec2 aTexCoords;

out vec3 outColor;
out vec2 outTexCoords;

uniform mat4 model;
uniform mat4 proj_view;

void main(){
    // gl_Position = vec4(aPos, 1.0);
    gl_Position = (proj_view * model) * vec4(aPos, 1.0);
    outColor = aColor;
    outTexCoords = aTexCoords;
}