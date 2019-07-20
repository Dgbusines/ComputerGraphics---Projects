#include <stb_image.h>
#include <glm/glm.hpp>
#include <bits/stdc++.h>
#include <vector>
using namespace std;

class COBJ{
    public:
        float *objModel;
        float matAmb[3];
        float matDiff[3];
        float matSpec[3];
        float traslation[3];
        int sizeModel;
        COBJ();
        ~COBJ();
        bool load(char *path);
        void draw();
};
