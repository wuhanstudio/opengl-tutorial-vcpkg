//-----------------------------------------------------------------------------
// lighting_point.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader for point light
//-----------------------------------------------------------------------------
#version 330 core

struct Material 
{
    vec3 ambient;
    sampler2D diffuseMap;
    vec3 specular;
    float shininess;
};

struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float exponent;
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform PointLight pointLight;
uniform Material material;
uniform vec3 viewPos;

out vec4 frag_color;

void main()
{
    // Ambient ------------------------------------------------------------------------------
	vec3 ambient = pointLight.ambient * material.ambient * vec3(texture(material.diffuseMap, TexCoord));
  	
    // Diffuse ------------------------------------------------------------------------------
    vec3 normal = normalize(Normal); 
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord));
    
     // Specular - Blinn-Phong --------------------------------------------------------------
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0);
	vec3 specular = pointLight.specular * material.specular * pow(NDotH, material.shininess);
	
	// Attenuation using Kc, Kl, Kq ---------------------------------------------------------
	float d = length(pointLight.position - FragPos);  // distance to light
	float attenuation = 1.0f / (pointLight.constant + pointLight.linear * d + pointLight.exponent * (d * d));

	diffuse *= attenuation;
	specular *= attenuation;
	
	frag_color = vec4(ambient + diffuse + specular, 1.0f);
};