//-----------------------------------------------------------------------------
// lighting_phong.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader for Blinn-Phong lighting
//-----------------------------------------------------------------------------
#version 300 es
precision mediump float;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

in vec4 FragPosLightSpace;

uniform sampler2D texture_map;
uniform sampler2D shadowMap;

uniform vec3 lightPos;			// for diffuse
uniform vec3 lightColor;		// for diffuse
uniform vec3 viewPos;			// for specular

out vec4 frag_color;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // check whether current frag pos is in shadow
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    // Ambient ---------------------------------------------------------
    float ambientFactor = 0.1f;
    vec3 ambient = lightColor * ambientFactor;
  	
    // Diffuse ------------------------------------
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float NDotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * NDotL;
    
    // Specular (Blinn-Phong)-------------------------------------------
	float specularFactor = 0.8f;
	float shininess = 32.0f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float NDotH = max(dot(normal, halfDir), 0.0);
	vec3 specular = lightColor * specularFactor * pow(NDotH, shininess);
	
	vec4 texel = texture(texture_map, TexCoord);

    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);  

    vec3 color = texture(texture_map, TexCoord).rgb;
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    frag_color = vec4(lighting, 1.0);
}
