#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 Color;

out vec3 Normal;
out vec3 FragPos;
out vec3 ObjectColor;

void main(){
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    
    FragPos = vec3(M * vec4(vertexPosition_modelspace, 1));
    Normal = normalize(mat3(transpose(inverse(M))) * normal);
    ObjectColor = Color;
}
