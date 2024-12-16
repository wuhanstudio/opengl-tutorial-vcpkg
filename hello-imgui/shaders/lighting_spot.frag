//-----------------------------------------------------------------------------
// lighting_spot.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader for spot light
//-----------------------------------------------------------------------------
#version 330 core

struct Material 
{
    vec3 ambient;
    sampler2D diffuseMap;
    vec3 specular;
    float shininess;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cosInnerCone;
	float cosOuterCone;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int on;

	float constant;
	float linear;
	float exponent;
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

out vec4 frag_color;

vec3 calcSpotLight();

void main()
{
    // Ambient -------------------------------------------------------------------------
	vec3 ambient = spotLight.ambient * material.ambient * vec3(texture(material.diffuseMap, TexCoord));
	
	vec3 spotColor = vec3(0.0f);

	// If the light isn't on then just return 0 for diffuse and specular colors
	if (spotLight.on == 1)
		spotColor = calcSpotLight();

	frag_color = vec4(ambient + spotColor, 1.0f);
}; 

//--------------------------------------------------------------
// Calculate the spotlight effect and return the resulting 
// diffuse and specular color summation
//--------------------------------------------------------------
vec3 calcSpotLight()
{
	vec3 lightDir = normalize(spotLight.position - FragPos);
	vec3 spotDir  = normalize(spotLight.direction);

	float cosDir = dot(-lightDir, spotDir);  // angle between the lights direction vector and spotlights direction vector
	float spotIntensity = smoothstep(spotLight.cosOuterCone, spotLight.cosInnerCone, cosDir);

	// Diffuse ------------------------------------------------------------------------- 
    vec3 normal = normalize(Normal);  
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord));
    
     // Specular - Blinn-Phong ----------------------------------------------------------
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0f);
	vec3 specular = spotLight.specular * material.specular * pow(NDotH, material.shininess);

	// Attenuation using Kc, Kl, Kq -----------------------------------------------------
	float d = length(spotLight.position - FragPos);  // distance to light
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear * d + spotLight.exponent * (d * d));

	diffuse *= attenuation * spotIntensity;
	specular *= attenuation * spotIntensity;
	
	return (diffuse + specular);
}