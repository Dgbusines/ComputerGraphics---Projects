#version 330 core
layout (location = 0) in vec3 vertexPosition;
//Textures
layout (location = 1) in vec2 texco;

// Vertex data out data
uniform mat4 MVP;
out vec2 texcord;

void main()
{
    texcord = texco;
    gl_Position =  MVP * vec4(vertexPosition,1);
}