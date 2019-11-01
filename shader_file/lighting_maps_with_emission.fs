#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    sampler2D diffuseTex;
    sampler2D specularTex;
    sampler2D emissionTex;
    float shininess;
};

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
    // ambient 环境光
    vec3 ambient = light.ambient * texture(material.diffuseTex, TexCoord).rgb;
  	
    // diffuse 漫反射
    vec3 norm = normalize(Normal);
    // 这个光方向是指朝着光源的方向
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseTex, TexCoord).rgb;
    
    // specular 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specularTex, TexCoord).rgb;
    //vec3 specular = light.specular * (spec * vec3(1.0,1.0,1.0));

    // emission 自发光
    vec3 emission = texture(material.emissionTex, TexCoord).rgb;

    vec3 result = ambient + diffuse + specular + emission;

    //vec3 result = ambientStrength * vec3(1,1,1) * objectColor;
    
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(1.0f,0,0,1.0f);
} 