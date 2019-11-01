#version 330 core

out vec4  fColor;

in vec2 TexCoords;

uniform sampler2D diffuseTex;

void main()
{
    //fColor = texture(diffuseTex, TexCoords);
    fColor = vec4(1,0,0,1);
}
