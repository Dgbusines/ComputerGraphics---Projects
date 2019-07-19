#version 330 core
//Vertexs
layout (location = 0) in vec3 vertexPosition;
//Textures
layout (location = 1) in vec2 texco;
//Normals
layout (location = 2) in vec3 normals;
//Normals
layout (location = 3) in vec3 tangents;
//Normals
layout (location = 4) in vec3 bitangents;

// Vertex data out data
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;


out VS_OUT {
    vec3 vP;
    vec2 texcord;
    vec3 nM;
	vec4 FragPosLightSpace;
} vs_out;

out vec3 T;
out vec3 N;
out vec3 B;

void main()
{
    vs_out.vP = vec3(model * vec4(vertexPosition, 1.0));    
    vs_out.texcord = texco;
    vs_out.nM = mat3(transpose(inverse(model))) * normals;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.vP, 1.0);	//Shadow Map

    //Data for normal mapping
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    T = normalize(normalMatrix * tangents);
    N = normalize(normalMatrix * normals);
    B = cross(N, T);
    
    gl_Position =  projection * view * vec4(vs_out.vP, 1.0f);
}
