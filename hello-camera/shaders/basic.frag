//-----------------------------------------------------------------------------
// basic_part2.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader for Part 2 of this tutorial
//-----------------------------------------------------------------------------
#version 330 core

in vec2 TexCoord;
out vec4 frag_color;

uniform sampler2D texSampler1;
uniform sampler2D texSampler2;

void main()
{
	frag_color = mix(texture(texSampler1, TexCoord), texture(texSampler2, TexCoord), 0.2);
}
