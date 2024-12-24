//-----------------------------------------------------------------------------
// basic.vert by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Vertex shader
//-----------------------------------------------------------------------------
#version 300 es
precision mediump float;

layout (location = 0) in vec3 pos;

uniform vec2 posOffset;

void main()
{
	gl_Position = vec4(pos.x + posOffset.x, pos.y + posOffset.y, pos.z, 1.0);
}