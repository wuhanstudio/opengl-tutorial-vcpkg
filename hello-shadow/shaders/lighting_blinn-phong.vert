//-----------------------------------------------------------------------------
// lighting_phong.vert by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Vertex shader for phong lighting
//-----------------------------------------------------------------------------
#version 330 core

layout (location = 0) in vec3 pos;			
layout (location = 1) in vec3 normal;	
layout (location = 2) in vec2 texCoord;

uniform mat4 model;			// model matrix
uniform mat4 view;			// view matrix
uniform mat4 projection;	// projection matrix

uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

out vec4 FragPosLightSpace;

void main()
{
    FragPos = vec3(model * vec4(pos, 1.0f));			// vertex position in world space
	Normal = normal;
	
	TexCoord = texCoord;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

	gl_Position = projection * view *  model * vec4(pos, 1.0f);
}