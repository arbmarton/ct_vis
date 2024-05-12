#version 410 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler3D texture3D;
uniform float zLevel;
uniform vec3 forward;

void main()
{ 
	const vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
	const vec3 up = normalize(cross(right, forward));
	const vec3 center = vec3(0.5, 0.5, 0.5);

	const vec3 samplingPosition = center + 
		right * (TexCoords.x * 2.0 - 1.0) * 0.5 + 
		up * ((1.0 - TexCoords.y) * 2.0 - 1.0) * 0.5 +
		forward * (zLevel * 2.0 - 1.0) * 0.5;

	const vec4 sampled = texture(texture3D, samplingPosition);
	FragColor = vec4(sampled.r, sampled.r, sampled.r, 1.0);
}
