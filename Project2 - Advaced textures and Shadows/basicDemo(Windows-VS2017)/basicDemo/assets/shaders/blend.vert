#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 texco;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = texco;
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
