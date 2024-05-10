#version 460 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler3D screenTexture;
uniform float zLevel;

void main()
{ 
	vec4 sampled = texture(screenTexture, vec3(TexCoords.x, 1.0 - TexCoords.y, zLevel));
	FragColor = vec4(sampled.r, sampled.r, sampled.r, 1.0);
}