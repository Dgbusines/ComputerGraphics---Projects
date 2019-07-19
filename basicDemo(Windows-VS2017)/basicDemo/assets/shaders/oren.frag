#version 330 core
in vec2 texcord;
in vec3 vP;
in vec3 nM;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float roughness;
};

struct directLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct pointtLight {
    vec3 position1;
    vec3 position2;

    vec3 ambient1;
    vec3 diffuse1;
    vec3 specular1;
    vec3 ambient2;
    vec3 diffuse2;
    vec3 specular2;

    //Point
    float constant;
    float linear;
    float quadratic;
};
struct spottLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //Spot
    float cutOff;
    float outerCutOff;
    
    //Point
    float constant;
    float linear;
    float quadratic;
};

vec3 calculateDirLight ();
vec3 calculatePointLight ();
vec3 calculateSpotLight ();

out vec4 FragColor;
uniform sampler2D ourTexture;
uniform vec3 viewPos;
uniform Material material;
uniform directLight lightDirect;
uniform pointtLight lightPoint;
uniform spottLight lightSpot;
uniform int texLoad;
uniform int onOffDir;
uniform int onOffPoint;


//Difuse

void main()
{
    //Lights
    vec3 color = vec3(0.0f);

    if(onOffDir == 0) 
        color += vec3(0.0f);
    else
        color = calculateDirLight();
    if(onOffPoint == 0) 
        color += vec3(0.0f);
    else
        color += calculatePointLight();    


    //Final Color
    if (texLoad == 1)
        FragColor = texture(ourTexture, texcord) * vec4(color, 1.0);
    else
        FragColor = vec4(color, 1.0);
}

vec3 calculateDirLight() {
    //Directional
    //Ambient
    vec3 ambient = lightDirect.ambient * material.ambient;

    //Diffuse / Oren/Nayar
    vec3 lightDir = normalize(-lightDirect.direction);
    vec3 viewDir = normalize(viewPos - vP);
    vec3 normal = normalize(nM);
    
    float oren=max(dot(viewDir,nM),0.0f);
    float diff=max(dot(lightDir,nM),0.0f);
    vec3 VxN=viewDir-nM*oren;
    vec3 LxN=lightDir-nM*diff;
    
    float viewLightNorm=max(dot(normalize(VxN),normalize(LxN)),0.0f);

    float alpha=max(acos(diff),acos(oren));
    float beta=min(acos(diff),acos(oren));
    float roughness2=material.roughness*material.roughness;
    float A=1.0f-0.5f*(roughness2/(roughness2+0.57));
    float B=0.45f*roughness2/(roughness2+0.09f);
    diff = (A+(viewLightNorm*B*sin(alpha)*tan(beta)));

    vec3 diffuse = lightDirect.diffuse * diff * material.diffuse;

    return (ambient + diffuse);
}

vec3 calculatePointLight() {

    //Point - First Light
    //Ambient
    vec3 ambient = lightPoint.ambient1 * material.ambient;

    //Diffuse - Oren/Nayar
    vec3 lightDir = normalize(lightPoint.position1 - vP);
    vec3 viewDir = normalize(viewPos - vP);
    vec3 normal = normalize(nM);
    
    float oren=max(dot(viewDir,nM),0.0f);
    float diff=max(dot(lightDir,nM),0.0f);
    vec3 VxN=viewDir-nM*oren;
    vec3 LxN=lightDir-nM*diff;
    
    float viewLightNorm=max(dot(normalize(VxN),normalize(LxN)),0.0f);

    float alpha=max(acos(diff),acos(oren));
    float beta=min(acos(diff),acos(oren));
    float roughness2=material.roughness*material.roughness;
    float A=1.0f-0.5f*(roughness2/(roughness2+0.57));
    float B=0.45f*roughness2/(roughness2+0.09f);
    diff = (A+(viewLightNorm*B*sin(alpha)*tan(beta)));

    vec3 diffuse = lightPoint.diffuse1 * diff * material.diffuse;

    //Attenuation
    float distance1    = length(lightPoint.position1 - vP);
    float attenuation = 1.0 / (lightPoint.constant + lightPoint.linear * distance1 + lightPoint.quadratic * (distance1 * distance1));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;

    vec3 color = (ambient + diffuse);

    //Second Point
    //Diffuse - Oren/Nayar
    ambient = lightPoint.ambient2 * material.ambient;
    lightDir = normalize(lightPoint.position2 - vP);    
    diff=max(dot(lightDir,nM),0.0f);
    LxN=lightDir-nM*diff;
    
    viewLightNorm=max(dot(normalize(VxN),normalize(LxN)),0.0f);

    alpha=max(acos(diff),acos(oren));
    beta=min(acos(diff),acos(oren));
    roughness2=material.roughness*material.roughness;
    A=1.0f-0.5f*(roughness2/(roughness2+0.57));
    B=0.45f*roughness2/(roughness2+0.09f);
    diff = (A+(viewLightNorm*B*sin(alpha)*tan(beta)));

    diffuse = lightPoint.diffuse2 * diff * material.diffuse;

    distance1    = length(lightPoint.position2 - vP);
    attenuation = 1.0 / (lightPoint.constant + lightPoint.linear * distance1 + lightPoint.quadratic * (distance1 * distance1));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;

    color += (ambient + diffuse);


    //Final Color
    return color;
}

vec3 calculateSpotLight() {
    //Spot
    //Ambient
    vec3 ambient = lightSpot.ambient * material.ambient;

    //Diffuse - Oren/Nayar
    vec3 lightDir = normalize(lightSpot.position - vP);
    vec3 viewDir = normalize(viewPos - vP);
    vec3 normal = normalize(nM);
    
    float oren=max(dot(viewDir,nM),0.0f);
    float diff=max(dot(lightDir,nM),0.0f);
    vec3 VxN=viewDir-nM*oren;
    vec3 LxN=lightDir-nM*diff;
    
    float viewLightNorm=max(dot(normalize(VxN),normalize(LxN)),0.0f);

    float alpha=max(acos(diff),acos(oren));
    float beta=min(acos(diff),acos(oren));
    float roughness2=material.roughness*material.roughness;
    float A=1.0f-0.5f*(roughness2/(roughness2+0.57));
    float B=0.45f*roughness2/(roughness2+0.09f);
    diff = (A+(viewLightNorm*B*sin(alpha)*tan(beta)));

    vec3 diffuse = lightDirect.diffuse * diff * material.diffuse;

    //Spotlight
    float theta = dot(lightDir, normalize(-lightSpot.direction)); 
    float epsilon = (lightSpot.cutOff - lightSpot.outerCutOff);
    float intensity = clamp((theta - lightSpot.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;

    //Attenuation
    float distance1    = length(lightSpot.position - vP);
    float attenuation = 1.0 / (lightSpot.constant + lightSpot.linear * distance1 + lightSpot.quadratic * (distance1 * distance1));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;

    //Final Color
    return(ambient + diffuse);
}