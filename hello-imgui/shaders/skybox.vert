#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main()
{
	TexCoords = pos;
	vec4 pos = projection * view * vec4(pos, 1.0f);
	gl_Position = pos.xyww;
}
