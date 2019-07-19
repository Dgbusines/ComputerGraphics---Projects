#include "objLoader.h"

COBJ::COBJ(){
    matAmb[0] = 1.0f;matDiff[0] = 1.0f;matSpec[0] = 1.0f;
	matAmb[1] = 1.0f;matDiff[1] = 1.0f;matSpec[1] = 1.0f;
	matAmb[2] = 1.0f;matDiff[2] = 1.0f;matSpec[2] = 1.0f;
	traslation[2] = 0.0f;traslation[2] = 0.0f;traslation[2] = 0.0f;
}
COBJ::~COBJ(){
}

bool COBJ::load(char* path) {
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }

    while( 1 ) {

        char lineHeader[128];
        // Lee la primera palabra de la línea
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File, es decir, el final del archivo. Se finaliza el ciclo.

        // else : analizar el lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 ) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
        }
        else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }

        else if ( strcmp( lineHeader, "f" ) == 0 ) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    sizeModel = 14*vertexIndices.size();
    objModel = new float[sizeModel];
    int pos = 0;
    glm::vec3 vertex[3];
    glm::vec2 uv[3];
    glm::vec3 norms[3];

    for( unsigned int i=0;i<vertexIndices.size()-2; i=i+3 ){

        //Calculate First Vertex
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normIndex = normalIndices[i];
        vertex[0] = temp_vertices[ vertexIndex-1 ];
        uv[0] = temp_uvs[ uvIndex-1 ];
        norms[0] = temp_normals[ normIndex-1 ];

        //Calculate Second Vertex
        vertexIndex = vertexIndices[i+1];
        uvIndex = uvIndices[i+1];
        normIndex = normalIndices[i+1];
        vertex[1] = temp_vertices[ vertexIndex-1 ];
        uv[1] = temp_uvs[ uvIndex-1 ];
        norms[1] = temp_normals[ normIndex-1 ];

        //Calculate Third Vertex
        vertexIndex = vertexIndices[i+2];
        uvIndex = uvIndices[i+2];
        normIndex = normalIndices[i+2];
        vertex[2] = temp_vertices[ vertexIndex-1 ];
        uv[2] = temp_uvs[ uvIndex-1 ];
        norms[2] = temp_normals[ normIndex-1 ];

        //This is for normal mapping
        glm::vec3 tangent1, bitangent1;
        // triangle 1
        glm::vec3 edge1 = vertex[1] - vertex[0];
        glm::vec3 edge2 = vertex[2] - vertex[0];
        glm::vec2 deltaUV1 = uv[1] - uv[0];
        glm::vec2 deltaUV2 = uv[2] - uv[0];

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        //Vertexs
        objModel[pos] = vertex[0].x;
        objModel[pos+1] = vertex[0].y;
        objModel[pos+2] = vertex[0].z;
        objModel[pos+14] = vertex[1].x;
        objModel[pos+15] = vertex[1].y;
        objModel[pos+16] = vertex[1].z;
        objModel[pos+28] = vertex[2].x;
        objModel[pos+29] = vertex[2].y;
        objModel[pos+30] = vertex[2].z;

        //Textures
        objModel[pos+3] = uv[0].x;
        objModel[pos+4] = uv[0].y;
        objModel[pos+17] = uv[1].x;
        objModel[pos+18] = uv[1].y;
        objModel[pos+31] = uv[2].x;
        objModel[pos+32] = uv[2].y;

        //Normals
        objModel[pos+5] = norms[0].x;
        objModel[pos+6] = norms[0].y;
        objModel[pos+7] = norms[0].z;
        objModel[pos+19] = norms[1].x;
        objModel[pos+20] = norms[1].y;
        objModel[pos+21] = norms[1].z;
        objModel[pos+33] = norms[2].x;
        objModel[pos+34] = norms[2].y;
        objModel[pos+35] = norms[2].z;

        //Tangents and bitangents
        objModel[pos+8] = objModel[pos+22] = objModel[pos+36] = tangent1.x;
        objModel[pos+9] = objModel[pos+23] = objModel[pos+37] = tangent1.y;
        objModel[pos+10] = objModel[pos+24] = objModel[pos+38] = tangent1.z;
        objModel[pos+11] = objModel[pos+25] = objModel[pos+39] = bitangent1.x;
        objModel[pos+12] = objModel[pos+26] = objModel[pos+40] = bitangent1.y;
        objModel[pos+13] = objModel[pos+27] = objModel[pos+41] = bitangent1.z;
        pos += 42;

        if (i + 3 == vertexIndices.size() - 2) --i;
        else if(i + 3 == vertexIndices.size() - 1) i-=2;
        
    }


    return true;
}

void COBJ::draw() {


}
