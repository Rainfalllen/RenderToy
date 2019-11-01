#version 330 core
out vec4 FragColor;
in vec3 TexCoord;
uniform samplerCube skyboxTex;

void main()
{
    vec3 color = texture(skyboxTex,TexCoord).rgb;

    // // HDR tonemapping
    // color = color / (color + vec3(1.0));
    // // gamma correct
    // color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}
