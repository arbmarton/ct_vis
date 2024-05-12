#version 460 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D viewport1;
uniform sampler2D viewport2;
uniform sampler2D viewport3;

void main()
{ 
    if (TexCoords.x < 0.5 && TexCoords.y >= 0.5) {
        float x = TexCoords.x * 2.0;
        float y = (TexCoords.y - 0.5) * 2.0;
        FragColor = texture(viewport1, vec2(x, y));
    } 
    else if (TexCoords.x >= 0.5 && TexCoords.y >= 0.5) {
        float x = (TexCoords.x - 0.5) * 2.0;
        float y = (TexCoords.y - 0.5) * 2.0;
        FragColor = texture(viewport2, vec2(x, y));
    }
    else if (TexCoords.x < 0.5 && TexCoords.y < 0.5) {
        float x = TexCoords.x * 2.0;
        float y = TexCoords.y * 2.0;
        FragColor = texture(viewport3, vec2(x, y));
    }
    else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    //FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}
