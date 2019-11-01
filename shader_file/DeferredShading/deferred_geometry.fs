#version 330 core
// layout (location = 0) out vec3 gPosition;
// layout (location = 1) out vec3 gNormal;
// layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;

out vec4 color;

void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    // gPosition = FragPos;
    // // Also store the per-fragment normals into the gbuffer
    // gNormal = normalize(Normal);
    // // And the diffuse per-fragment color
    // gAlbedoSpec.rgb = texture(diffuseTex, TexCoords).rgb;
    // // Store specular intensity in gAlbedoSpec's alpha component
    // gAlbedoSpec.a = texture(specularTex, TexCoords).r;

    color = vec4(1,1,1,1);
}