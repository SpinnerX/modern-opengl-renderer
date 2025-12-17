#version 330 core
out vec4 FragColor;

in vec3 outColor;
in vec2 outTexCoords;

uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normal;

void main(){
    // FragColor = vec4(outColor, 1.0);
    // vec3 ambient_base = vec3(texture(ambient, outTexCoords));
    // vec3 diffuse_base = vec3(texture(ambient, outTexCoords));
    FragColor = texture(ambient, outTexCoords);
}
