#version 460 core

layout (location = 0) out vec4 alphaness;
layout (location = 1) out vec4 color;

in vec2 TexCoords;

uniform sampler2D colorTex;
uniform sampler2D alphanessTex;


void main() {
	// copy the texture
	color = texture(colorTex, TexCoords);
	alphaness = texture(alphanessTex, TexCoords);
}