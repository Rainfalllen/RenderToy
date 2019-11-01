#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;  

struct Material
{
    sampler2D diffuseTex;
    sampler2D specularTex;
    //sampler2D emissionTex;
    float shininess;
};

// 方向光
struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 点光源
struct PointLight
{
    vec3 position;
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 聚光
struct SpotLight
{
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float cosPhyInner;
	float cosPhyOutter;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;

uniform DirectionalLight dirLight;
uniform PointLight pointLights[4];
uniform SpotLight spotLight;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{   
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 color = CalcDirectionalLight(dirLight, norm, viewDir) * 2.8;
    // for(int i = 0; i < 4; i++){
    //     result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // }
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir); 
    
    // emission 自发光
    // vec3 emission = texture(material.emissionTex, TexCoord).rgb;
    //vec3 result = ambient + diffuse + specular + emission;

    // color = color / (color + vec3(1.0));
    // color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
 
} 


vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    // 日常取反
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * vec3(texture(material.diffuseTex, TexCoord));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseTex, TexCoord));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specularTex, TexCoord));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float dis = length(light.position - fragPos);
    // 衰减度
    float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * dis * dis);

    vec3 ambient = light.ambient * vec3(texture(material.diffuseTex, TexCoord));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseTex, TexCoord));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specularTex, TexCoord));

    return ((ambient + diffuse + specular) * attenuation);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float dis = length(light.position - fragPos);
    // 衰减度
    float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * dis * dis);

    // 聚光特性
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cosPhyInner - light.cosPhyOutter;
    float intensity = clamp((theta - light.cosPhyOutter) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuseTex, TexCoord));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseTex, TexCoord));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specularTex, TexCoord));

    return ((ambient + diffuse + specular) * attenuation * intensity);
}