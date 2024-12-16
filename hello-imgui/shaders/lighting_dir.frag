//-----------------------------------------------------------------------------
// lighting_dir.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader for directional light
//-----------------------------------------------------------------------------
#version 330 core

struct Material 
{
    vec3 ambient;
    sampler2D diffuseMap;
    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
  
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform DirectionalLight dirLight;
uniform Material material;
uniform vec3 viewPos;

out vec4 frag_color;

void main()
{ 
    // Ambient -------------------------------------------------------------------------
	vec3 ambient = dirLight.ambient * material.ambient * vec3(texture(material.diffuseMap, TexCoord));
  	
    // Diffuse -------------------------------------------------------------------------
    vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(-dirLight.direction);  // negate => Must be a direction from fragment towards the light
	float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord));
    
    // Specular - Blinn-Phong ----------------------------------------------------------
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0);
	vec3 specular = dirLight.specular * material.specular * pow(NDotH, material.shininess);
		
    frag_color = vec4(ambient + diffuse + specular, 1.0f);
}
