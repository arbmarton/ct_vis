#version 410 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler3D texture3D;
uniform float zLevel;
uniform vec3 forward;
uniform float minWindow;
uniform float maxWindow;

void main()
{ 
	vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
	vec3 up = normalize(cross(right, forward));
	vec3 center = vec3(0.5, 0.5, 0.5);

	vec3 samplingPosition = center + 
		right * (TexCoords.x * 2.0 - 1.0) * 0.5 + 
		up * ((1.0 - TexCoords.y) * 2.0 - 1.0) * 0.5 +
		forward * (zLevel * 2.0 - 1.0) * 0.5;

	vec4 sampled = texture(texture3D, samplingPosition);

	float temp = max(sampled.r, minWindow);
	float modifiedValue = min(temp, maxWindow);
	float interpolation = (modifiedValue - minWindow) / (maxWindow - minWindow);

	FragColor = vec4(interpolation, interpolation, interpolation, 1.0);
}
