//-----------------------------------------------------------------------------
// lighting_phong_materials.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader for Blinn-Phong lighting with materials
//-----------------------------------------------------------------------------
#version 330 core

struct Material 
{
    vec3 ambient;
    sampler2D diffuseMap;
    vec3 specular;
    float shininess;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
  
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

out vec4 frag_color;

void main()
{ 
    // Ambient -------------------------------------------------------------------------
    vec3 ambient = light.ambient * material.ambient;
  	
    // Diffuse -------------------------------------------------------------------------
    vec3 normal = normalize(Normal); 
    vec3 lightDir = normalize(light.position - FragPos);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord));
    
    // Specular - Blinn-Phong ----------------------------------------------------------
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0);
	vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess);
	
    frag_color = vec4(ambient + diffuse + specular, 1.0f);
}
