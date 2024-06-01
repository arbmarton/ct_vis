#version 410 core

out float SampledValue;
  
in vec2 TexCoords;

uniform sampler3D texture3D;
uniform float zLevel;
uniform float fov;
uniform vec3 forward;
uniform vec3 upVector;
uniform vec3 centerOffset;

vec3 center = vec3(0.5, 0.5, 0.5) + centerOffset;

void main()
{ 
	vec3 right = normalize(cross(forward, upVector)) * fov;
	vec3 up = normalize(cross(right, forward)) * fov;

	vec3 samplingPosition = center + 
		right * (TexCoords.x * 2.0 - 1.0) * 0.5 + 
		up * ((TexCoords.y) * 2.0 - 1.0) * 0.5 +
		forward * (zLevel * 2.0 - 1.0) * 0.5;

	vec4 sampled;
	if (any(greaterThan(samplingPosition, vec3(1.0))) || any(lessThan(samplingPosition, vec3(0.0)))) {
		// This results in black color for pixels outside the volume
		sampled = vec4(-20000);
	} 
	else
	{
		sampled = texture(texture3D, samplingPosition);
	}

	SampledValue = sampled.r;
}
