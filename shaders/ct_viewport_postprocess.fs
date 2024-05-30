#version 410 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D textureInput;
uniform float zLevel;
uniform vec3 forward;
uniform vec3 viewportColor;
uniform float minWindow;
uniform float maxWindow;

uniform vec3 otherColor1;
uniform vec3 otherColor2;
uniform vec3 otherForward1;
uniform vec3 otherForward2;
uniform float otherZ1;
uniform float otherZ2;

float borderWidth = 0.005f;
float otherViewportLinewidth = 0.001f;
float overlayOpacity = 0.75f;

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