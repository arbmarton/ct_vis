#version 410 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform bool horizontal;
uniform int size;
uniform float weights[32];

void main()
{             
     vec2 tex_offset = 1.0 / textureSize(screenTexture, 0);
     vec3 result = texture(screenTexture, TexCoords).rgb * weights[0];

     if (horizontal)
     {
         for (int i = 1; i < size; ++i)
         {
            result += texture(screenTexture, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            result += texture(screenTexture, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
         }
     }
     else
     {
         for (int i = 1; i < size; ++i)
         {
             result += texture(screenTexture, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
             result += texture(screenTexture, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
         }
     }

     FragColor = vec4(result, 1.0);
}