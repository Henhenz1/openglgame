/*
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aInstanceMatrix;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}
*/

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0f); 
}