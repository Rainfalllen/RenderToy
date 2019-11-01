#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
//uniform vec3 objectColor;

uniform sampler2D texture1;

void main()
{
    // ambient 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 漫反射
    vec3 norm = normalize(Normal);
    // 这个光方向是指朝着光源的方向
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular 镜面反射
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    vec3 objectColor = texture(texture1, TexCoord).xyz;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    //vec3 result = ambientStrength * vec3(1,1,1) * objectColor;
    
    FragColor = vec4(result, 1.0);
} 