#version 460 core

out vec4 screenShot;

in vec2 TexCoords;

uniform sampler2D screen;

void main() {
	vec2 coord = vec2(TexCoords.x, 1 - TexCoords.y);
	// copy the texture
	screenShot = texture(screen, coord);
}