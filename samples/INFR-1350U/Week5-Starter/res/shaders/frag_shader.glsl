#version 410

layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inUV;
layout(location = 3) in vec3 inNormal;


out vec4 frag_color;

void main() { 
	
	frag_color = vec4(1.0);
}