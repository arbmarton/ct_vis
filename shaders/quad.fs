#version 460 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
	vec4 sampled = texture(screenTexture, vec2(TexCoords.x, 1.0 - TexCoords.y));
	FragColor = vec4(sampled.r, sampled.r, sampled.r, 1.0);
}