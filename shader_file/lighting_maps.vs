#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 一般等比缩放时不采用此方式
    // Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    Normal = aNormal;
    Normal = vec3(model * vec4(Normal, 1));

    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

