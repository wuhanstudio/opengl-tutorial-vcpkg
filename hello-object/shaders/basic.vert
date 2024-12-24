//-----------------------------------------------------------------------------
// basic.vert by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Vertex shader
//-----------------------------------------------------------------------------
#version 300 es 
precision mediump float;

layout (location = 0) in vec3 pos;  // in local coords
layout (location = 1) in vec2 texCoord;

precision mediump float;
out vec2 TexCoord;

uniform mat4 model;			// model matrix
uniform mat4 view;			// view matrix
uniform mat4 projection;	// projection matrix

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	TexCoord = texCoord;
}
