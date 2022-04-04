#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model1;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    Normal = mat3(transpose(inverse(model1))) * aNormal;
    FragPos = vec3(model1 * vec4(aPos, 1.0));
    gl_Position = projection * view  * model1 * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}