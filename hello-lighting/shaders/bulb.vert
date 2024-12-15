//-----------------------------------------------------------------------------
// bulb.vert by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Vertex shader for light bulb mesh
//-----------------------------------------------------------------------------
#version 330 core

layout (location = 0) in vec3 pos;			
layout (location = 1) in vec3 normal;	
layout (location = 2) in vec2 texCoord;

uniform mat4 model;			// model matrix
uniform mat4 view;			// view matrix
uniform mat4 projection;	// projection matrix

out vec2 TexCoord;

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	TexCoord = texCoord;
};