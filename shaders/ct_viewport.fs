#version 410 core

out float SampledValue;
  
in vec2 TexCoords;

uniform sampler3D texture3D;
uniform float zLevel;
uniform vec3 forward;

vec3 center = vec3(0.5, 0.5, 0.5);

void main()
{ 
	vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
	vec3 up = normalize(cross(right, forward));

	vec3 samplingPosition = center + 
		right * (TexCoords.x * 2.0 - 1.0) * 0.5 + 
		up * ((1.0 - TexCoords.y) * 2.0 - 1.0) * 0.5 +
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
