#version 330 core

uniform sampler2D texture;

out vec4 color;

void main(){
	color = texelFetch (texture, ivec2 (gl_FragCoord), 0);
}