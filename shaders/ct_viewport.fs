#version 410 core

out float SampledValue;
  
in vec2 TexCoords;

uniform sampler3D texture3D;
uniform float zLevel;
uniform float fov;
uniform vec3 forward;
uniform vec3 upVector;
uniform vec3 centerOffset;
uniform vec3 pixelSpacing;
uniform vec3 right;
uniform vec3 up;

vec3 center = vec3(0.5, 0.5, 0.5);

void main()
{ 
	vec3 samplingPosition = right * (TexCoords.x * 2.0 - 1.0) * 0.5 * fov + 
		up * (TexCoords.y * 2.0 - 1.0) * 0.5 * fov +
		forward * zLevel;
	samplingPosition /= pixelSpacing;
	samplingPosition += center + centerOffset / pixelSpacing;

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
