 //-----------------------------------------------------------------------------
// bulb.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader for light bulb mesh
//-----------------------------------------------------------------------------
#version 330 core

uniform vec3 lightColor;

out vec4 frag_color;

void main()
{
	frag_color = vec4(lightColor, 1.0f);
};
