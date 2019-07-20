#version 330 core
in VS_OUT {
    vec3 vP;
    vec2 texcord;
    vec3 nM;
	vec4 FragPosLightSpace;
} fs_in;

in vec3 T;
in vec3 B;
in vec3 N;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
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

vec3 calculateDirLight (vec2 texCoords);
vec3 calculatePointLight (vec2 texCoords);
vec3 calculateSpotLight (vec2 texCoords);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
float ShadowCalculation(vec3 lightPos, vec4 fragPosLightSpace);

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
uniform int onOffSpec;

//Tarea 2
uniform int reflection;
uniform float reflectance;
uniform int refraction;
uniform float refractAmb;
uniform float refractObj;
uniform samplerCube skybox;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D shadowMap;
uniform float heightScale;
uniform int parall;
uniform int getIn = 0;


mat3 TBN;
void main()
{
    //Lights
    vec3 color = vec3(0.0f);
    TBN = mat3(T, B, N);

    vec2 texCoords = fs_in.texcord;   
    if (texLoad == 1 && parall == 1 && getIn == 1) {
		TBN = transpose(TBN);
		vec3 ViewDir = normalize(TBN * viewPos - TBN * fs_in.vP);
		texCoords = ParallaxMapping(texCoords,  ViewDir); 
		if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
			discard;
	}

    if(onOffDir == 0) 
        color += vec3(0.0f);
    else {
        color = calculateDirLight(texCoords);
	}
    if(onOffPoint == 0) 
        color += vec3(0.0f);
    else
        color += calculatePointLight(texCoords);    
    if(onOffSpec == 0) 
        color += vec3(0.0f);
    else
        color += calculateSpotLight(texCoords);

    //Reflect or Refract

    //Final Color
    if (reflection == 1 || refraction == 1) {
        float ratio = refractAmb / refractObj;
        vec3 I = normalize(fs_in.vP - viewPos);
        vec3 Rl = reflect(I, fs_in.nM); //Reflection
        vec3 Rr = refract(I, fs_in.nM, ratio);  //Refraction
        if (reflection == 1) 
            FragColor = vec4(texture(skybox, reflectance*Rl).rgb, 1.0);
        if (refraction == 1) 
            FragColor = vec4(texture(skybox, Rr).rgb, 1.0);
    }
    else 
        FragColor = vec4(color, 1.0);
}

vec3 calculateDirLight(vec2 texCoords) {
    //Directional
    //Ambient
    vec3 ambient = lightDirect.ambient * material.ambient;
    if(texLoad == 1)
        ambient = lightDirect.ambient * material.ambient * texture(diffuseMap, texCoords).rgb;	

    //Diffuse
    vec3 lightDir = normalize(-lightDirect.direction);
	if(texLoad == 1 && parall == 1) 
		lightDir = normalize(TBN * -lightDirect.direction);

    vec3 normal = normalize(fs_in.nM);
    if(texLoad == 1) {
        normal = texture(normalMap, texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        if(parall != 1) normal = TBN * normal;
    }
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lightDirect.diffuse * diff * material.diffuse;
    if(texLoad == 1)
        diffuse = lightDirect.diffuse * diff * material.diffuse  * texture(diffuseMap, texCoords).rgb;

    //Specular
    /*
    //Phong
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    */
    //Blinn
        vec3 viewDir = normalize(viewPos - fs_in.vP);
		if (texLoad == 1 && parall == 1)
			viewDir = normalize(TBN * viewPos - TBN * fs_in.vP);
	
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        vec3 specular = lightDirect.specular * spec * material.specular;

    float shadow = ShadowCalculation(lightDirect.direction, fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(diffuseMap, texCoords).rgb;    
        
    FragColor = vec4(lighting, 1.0);
    return lighting;
}

vec3 calculatePointLight(vec2 texCoords)
{

    //Point - First Light
    //Ambient
    vec3 ambient = lightPoint.ambient1 * material.ambient;
    if(texLoad == 1) 
        ambient = lightPoint.ambient1 * material.ambient * texture(diffuseMap, texCoords).rgb;
        
    //Diffuse
    vec3 lightDir = normalize(lightPoint.position1 - fs_in.vP);
	if(texLoad == 1 && parall == 1) 
		lightDir = normalize(TBN * lightPoint.position1 - TBN * fs_in.vP);

    vec3 normal = normalize(fs_in.nM);
    if(texLoad == 1) {
        normal = texture(normalMap, texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        if(parall != 1) normal = TBN * normal;
    }
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lightPoint.diffuse1 * diff * material.diffuse;
    if(texLoad == 1)
        diffuse = lightPoint.diffuse1 * diff * material.diffuse  * texture(diffuseMap, texCoords).rgb;

    //Specular
    /*
    //Phong
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    */
    //Blinn
        vec3 viewDir = normalize(viewPos - fs_in.vP);
		if(texLoad == 1 && parall == 1) 
			viewDir = normalize(TBN * viewPos - TBN * fs_in.vP);
			
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        vec3 specular = lightPoint.specular1 * spec * material.specular;

    //Attenuation
    float distance1    = length(lightPoint.position1 - fs_in.vP);
	if(texLoad == 1 && parall == 1) 
		float distance1    = length(TBN * lightPoint.position1 - TBN * fs_in.vP);

    float attenuation = 1.0 / (lightPoint.constant + lightPoint.linear * distance1 + lightPoint.quadratic * (distance1 * distance1));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation; 

    vec3 color = (ambient + diffuse + specular);

    //Second Point
    ambient = lightPoint.ambient2 * material.ambient;
    if(texLoad == 1) 
        ambient = lightPoint.ambient2 * material.ambient * texture(diffuseMap, texCoords).rgb;

    lightDir = normalize(lightPoint.position2 - fs_in.vP);
	if(texLoad == 1 && parall == 1) 
		lightDir = normalize(TBN * lightPoint.position2 - TBN * fs_in.vP);

    diff = max(dot(lightDir, normal), 0.0);
    diffuse = lightPoint.diffuse2 * diff * material.diffuse;
    if(texLoad == 1)
        diffuse = lightPoint.diffuse2 * diff * material.diffuse  * texture(diffuseMap, texCoords).rgb;

    halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    specular = lightPoint.specular2 * spec * material.specular;

    distance1    = length(lightPoint.position2 - fs_in.vP);
	if(texLoad == 1 && parall == 1) 
		distance1    = length(TBN * lightPoint.position2 - TBN * fs_in.vP);

    attenuation = 1.0 / (lightPoint.constant + lightPoint.linear * distance1 + lightPoint.quadratic * (distance1 * distance1));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation; 


	color += (ambient + diffuse + specular);

    //Final Color

    return color;
}
vec3 calculateSpotLight(vec2 texCoords) 
{
    //Spot
    //Ambient
    vec3 ambient = lightSpot.ambient * material.ambient;
    if(texLoad == 1) 
        ambient = lightSpot.ambient * material.ambient * texture(diffuseMap, texCoords).rgb;

    //Diffuse
    vec3 lightDir = normalize(lightSpot.position - fs_in.vP);
	if(texLoad == 1 && parall == 1) 
		vec3 lightDir = normalize(TBN * lightSpot.position - TBN * fs_in.vP);

    vec3 normal = normalize(fs_in.nM);
    if(texLoad == 1) {
        normal = texture(normalMap, texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        if(parall != 1) normal = TBN * normal;
    }
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lightSpot.diffuse * diff * material.diffuse;
    if(texLoad == 1)
        vec3 diffuse = lightSpot.diffuse * diff * material.diffuse  * texture(diffuseMap, texCoords).rgb;

    //Specular
    /*
    //Phong
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    */
    //Blinn
        vec3 viewDir = normalize(viewPos - fs_in.vP);
		if(texLoad == 1 && parall == 1) 
			vec3 viewDir = normalize(TBN * viewPos - TBN * fs_in.vP);

        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        vec3 specular = lightSpot.specular * spec * material.specular;
        
    //Spotlight
    float theta = dot(lightDir, normalize(-lightSpot.direction));
	if(texLoad == 1 && parall == 1) 
		float theta = dot(lightDir, normalize(TBN * -lightSpot.direction));
	
    float epsilon = (lightSpot.cutOff - lightSpot.outerCutOff);
    float intensity = clamp((theta - lightSpot.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    //Attenuation
    float distance1    = length(lightSpot.position -  fs_in.vP);
	if(texLoad == 1 && parall == 1) 
		float distance1    = length(TBN * lightSpot.position -  TBN * fs_in.vP);

    float attenuation = 1.0 / (lightSpot.constant + lightSpot.linear * distance1 + lightSpot.quadratic * (distance1 * distance1));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;
	

    //Final Color
    return (ambient + diffuse + specular);
}
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    
	// calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    
	// depth of current layer
    float currentLayerDepth = 0.0;
    
	// the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        
		// get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
        
		// get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}
float ShadowCalculation(vec3 lightPos, vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.nM);
    vec3 lightDir = normalize(lightPos - fs_in.vP);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
