#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 outColor;
in vec3 outNormal;
in vec2 outTexCoords;

const int MAX_POINT_LIGHTS = 100;

struct point_light {
    vec4 position;
    vec4 color;
    float attenuation;
    float constant;
    float linear;
    float quadratic;
    vec4 ambient; // 4th channel is the intensity value
    vec4 diffuse;
    vec4 specular;
};

uniform int num_point_lights;
uniform point_light lights[MAX_POINT_LIGHTS];
uniform sampler2D ambient_texture;
uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D normal;


vec3 calc_point_light(point_light light, vec3 normal, vec3 fragPos, vec3 view_dir) {
    vec3 pos = vec3(light.position);
    vec3 dir_to_light = normalize(pos - fragPos);
    float diff = max(dot(normal, dir_to_light), 0.0);
    
    vec3 reflect_dir = reflect(-dir_to_light, normal);
    
    float shininess = 32.0; // replace with material.shininess
    
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    
    float dist = length(pos - fragPos);
    float attenuation = light.attenuation / (light.constant + light.linear * dist + light.quadratic * pow(dist, 2));
    
    vec3 ambient = light.ambient.rgb * vec3(texture(diffuse_texture, outTexCoords));
    vec3 diffuse = light.diffuse.xyz * diff * vec3(texture(diffuse_texture, outTexCoords));
    vec3 specular = light.specular.xyz * spec * vec3(texture(specular_texture, outTexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
/*
vec3 calc_point_light(point_light light, vec3 normal, vec3 fragPos, vec3 view_dir) {
    vec3 light_pos = vec3(light.position);
    vec3 dir_to_light = normalize(light_pos - fragPos);

    float diff = max(dot(normal, dir_to_light), 0.0);

    vec3 reflect_dir = reflect(-dir_to_light, normal);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    float dist = length(light_pos - fragPos);
    float attenuation = light.attenuation / (light.constant + light.linear * dist + light.quadratic * pow(dist, 2));

    vec3 ambient = light.ambient.rgb * vec3(texture(diffuse_texture, outTexCoords));
    vec3 diffuse = light.diffuse.xyz * diff * vec3(texture(diffuse_texture, outTexCoords));
    vec3 specular = light.specular.xyz * spec * vec3(texture(specular_texture, outTexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
*/

void main(){
    // vec3 default_ambience = texture(ambient_texture, outTexCoords).rgb;
    vec3 default_ambience = (vec3(texture(diffuse_texture, outTexCoords)) * FragColor.rgb) * 0.1;
    vec3 normal = outNormal;
    
    // vec3 result = default_ambience;
    vec3 result = texture(ambient_texture, outTexCoords).rgb;
    
    int point_lights_count = int(min(num_point_lights, MAX_POINT_LIGHTS));
    for(int i = 0; i < point_lights_count; i++) {
        point_light source_light = lights[i];
        vec3 view_pos = source_light.position.xyz;
        vec3 dir_to_light = normalize(view_pos - FragPos.xyz);
        result += calc_point_light(source_light, normal, FragPos, dir_to_light) * (source_light.color.rgb * source_light.color.a);
    }


    // FragColor = default_ambience;
    FragColor = vec4(result, 1.0); 
    // FragColor = texture(ambient, outTexCoords);

}
