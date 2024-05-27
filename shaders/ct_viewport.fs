#version 410 core

out vec4 FragColor;
out float SampledValue;
  
in vec2 TexCoords;

uniform sampler3D texture3D;
uniform float zLevel;
uniform vec3 forward;
uniform float minWindow;
uniform float maxWindow;
uniform vec3 viewportColor;

uniform vec3 otherColor1;
uniform vec3 otherColor2;
uniform vec3 otherForward1;
uniform vec3 otherForward2;
uniform float otherZ1;
uniform float otherZ2;

float borderWidth = 0.01f;

vec3 center = vec3(0.5, 0.5, 0.5);

float calculateViewingAngleForOtherViewport(vec3 samplingPosition, vec3 otherForward, float otherZ)
{
	vec3 otherPoint = center + otherForward * (otherZ * 2.0 - 1.0) * 0.5;
	vec3 diff = otherPoint - samplingPosition;
	return dot(diff, otherForward);
}

void main()
{ 
	vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
	vec3 up = normalize(cross(right, forward));

	vec3 samplingPosition = center + 
		right * (TexCoords.x * 2.0 - 1.0) * 0.5 + 
		up * ((1.0 - TexCoords.y) * 2.0 - 1.0) * 0.5 +
		forward * (zLevel * 2.0 - 1.0) * 0.5;
;
	float dotProduct1 = calculateViewingAngleForOtherViewport(samplingPosition, otherForward1, otherZ1);
	float dotProduct2 = calculateViewingAngleForOtherViewport(samplingPosition, otherForward2, otherZ2);

	vec4 sampled;
	if (any(greaterThan(samplingPosition, vec3(1.0))) || any(lessThan(samplingPosition, vec3(0.0)))) {
		// This results in black color for pixels outside the volume
		sampled = vec4(-20000);
	} 
	else
	{
		sampled = texture(texture3D, samplingPosition);
	}

	if (TexCoords.x < borderWidth || TexCoords.x > (1 - borderWidth) || TexCoords.y < borderWidth || TexCoords.y > (1 - borderWidth)) {
		FragColor = vec4(viewportColor, 1.0);
	}
	else if (abs(dotProduct1) < 0.001) {
		FragColor = vec4(otherColor1, 1.0);
	}
	else if (abs(dotProduct2) < 0.001) {
		FragColor = vec4(otherColor2, 1.0);
	} else {
		float temp = max(sampled.r, minWindow);
		float modifiedValue = min(temp, maxWindow);
		float interpolation = (modifiedValue - minWindow) / (maxWindow - minWindow);

		FragColor = vec4(interpolation, interpolation, interpolation, 1.0);
	}

	SampledValue = sampled.r;
}
