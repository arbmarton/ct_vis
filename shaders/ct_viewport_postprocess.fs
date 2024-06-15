#version 410 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D textureInput;
uniform float zLevel;
uniform float fov;
uniform vec3 forward;
uniform vec3 viewportColor;
uniform float minWindow;
uniform float maxWindow;
uniform vec3 upVector;
uniform vec3 centerOffset;
uniform vec3 right;
uniform vec3 up;

uniform vec3 pixelSpacing;

uniform vec3 otherColor1;
uniform vec3 otherColor2;
uniform vec3 otherForward1;
uniform vec3 otherForward2;
uniform float otherZ1;
uniform float otherZ2;
uniform vec3 otherCenterOffset1;
uniform vec3 otherCenterOffset2;

float borderWidth = 0.005f;
float otherViewportLinewidth = 0.001f * fov;
float overlayOpacity = 0.75f;

vec3 center = vec3(0.5, 0.5, 0.5);

float calculateViewingAngleForOtherViewport(vec3 samplingPosition, vec3 otherCenterOffset, vec3 otherForward, float otherZ)
{
	vec3 otherPoint = center + otherCenterOffset + otherForward * otherZ / pixelSpacing;
	vec3 diff = otherPoint - samplingPosition;
	return dot(diff, otherForward);
}

void main()
{ 
	vec3 samplingPosition = right * (TexCoords.x * 2.0 - 1.0) * 0.5 * fov + 
		up * (TexCoords.y * 2.0 - 1.0) * 0.5 * fov +
		forward * zLevel;
	samplingPosition /= pixelSpacing;
	samplingPosition += center + centerOffset;

	float dotProduct1 = calculateViewingAngleForOtherViewport(samplingPosition, otherCenterOffset1, otherForward1, otherZ1);
	float dotProduct2 = calculateViewingAngleForOtherViewport(samplingPosition, otherCenterOffset2,  otherForward2, otherZ2);

	vec4 sampled = texture(textureInput, TexCoords);
	float temp = max(sampled.r, minWindow);
	float modifiedValue = min(temp, maxWindow);
	float interpolation = (modifiedValue - minWindow) / (maxWindow - minWindow);
	vec3 finalColor = vec3(interpolation, interpolation, interpolation);

	if (TexCoords.x < borderWidth || TexCoords.x > (1 - borderWidth) || TexCoords.y < borderWidth || TexCoords.y > (1 - borderWidth)) {
		vec3 borderColor = mix(finalColor, viewportColor, overlayOpacity);
		FragColor = vec4(borderColor, 1.0);
	}
	else if (abs(dotProduct1) < otherViewportLinewidth) {
		vec3 lineColor1 = mix(finalColor, otherColor1, overlayOpacity);
		FragColor = vec4(lineColor1, 1.0);
	}
	else if (abs(dotProduct2) < otherViewportLinewidth) {
		vec3 lineColor2 = mix(finalColor, otherColor2, overlayOpacity);
		FragColor = vec4(lineColor2, 1.0);
	} else {
		FragColor = vec4(finalColor, 1.0);
	}
}
