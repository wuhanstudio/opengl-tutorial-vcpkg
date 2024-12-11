//-----------------------------------------------------------------------------
// basic.frag by Steve Jones 
// Copyright (c) 2015-2019 Game Institute. All Rights Reserved.
//
// Fragment shader
//-----------------------------------------------------------------------------
#version 330 core

uniform vec4 vertColor;
out vec4 frag_color;

void main()
{
    frag_color = vertColor;
}
