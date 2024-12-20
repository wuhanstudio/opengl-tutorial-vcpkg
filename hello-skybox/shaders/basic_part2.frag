//-----------------------------------------------------------------------------
// basic_part2.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader for Part 2 of this tutorial
//-----------------------------------------------------------------------------
#version 330 core

in vec2 TexCoord;
out vec4 frag_color;

// #1 Method for setting samplers
uniform sampler2D texSampler1;
uniform sampler2D texSampler2;

// #2 Method for setting samplers (uncomment these lines and comment the previous two lines to try the second method
// Make sure to do the same in Textures_2.cpp and rebuild)
//layout (binding = 0) uniform sampler2D texSampler1;
//layout (binding = 1) uniform sampler2D texSampler2;

void main()
{
	// Texturing - Part 1
	//frag_color = texture(texSampler1, TexCoord);
	//frag_color = texelFetch(texSampler1, ivec2(gl_FragCoord.xy), 0);

	// Texturing - Part 2
	frag_color = mix(texture(texSampler1, TexCoord), texture(texSampler2, TexCoord), 0.2);
}
