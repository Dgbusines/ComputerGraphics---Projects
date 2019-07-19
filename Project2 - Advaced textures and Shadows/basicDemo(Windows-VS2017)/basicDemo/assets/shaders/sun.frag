#version 330 core
out vec4 FragColor;

uniform int onOffDir;

void main()
{
    if(onOffDir == 1) {
        FragColor = vec4(1.0f); // set all 4 vector values to 1.0
    }
    else {
        FragColor = vec4(0.0);
    }
}
