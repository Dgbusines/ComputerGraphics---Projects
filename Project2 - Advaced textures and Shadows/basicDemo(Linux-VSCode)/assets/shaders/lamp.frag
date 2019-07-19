#version 330 core
out vec4 FragColor;

uniform vec3 colLamp;
uniform int onOffPoint;

void main()
{
    if (onOffPoint == 1) {
        FragColor = vec4(colLamp, 1.0f); // set all 4 vector values to 1.0
    }
    else {
        FragColor = vec4(0.0);
    }
}
