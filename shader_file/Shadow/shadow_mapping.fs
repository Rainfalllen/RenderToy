#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;


uniform sampler2D diffuseTex;
uniform sampler2D specularTex;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float unpackDepth(const in vec4 rgbaDepth) 
{
    const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0*256.0), 1.0/(256.0*256.0*256.0));
    float depth = dot(rgbaDepth, bitShift);
    return depth;
}
float texture2DCompare(sampler2D depths, vec2 uv, float compare)
{
    float depth = unpackDepth(texture2D(depths, uv));
    // 如果depth < compare，返回0.0，否则返回1.0
    return 1.0 - step(compare, depth);
}
float texture2DCompare(sampler2D depths, vec2 uv, float compare, float bias)
{
    float depth = unpackDepth(texture2D(depths, uv));
    return 1.0 - step(compare - bias, depth);
}


float texture2DShadowLerp(sampler2D depths, vec2 uv, float compare, float bias){
    float size = 2048.0;
    vec2 texelSize = vec2(1.0) / vec2(2048.0, 2048.0);
    vec2 centroidUV = floor(uv * size + 0.5) / size;
    vec2 f = fract(uv * size + 0.5);
    float lb = texture2DCompare(depths, centroidUV + texelSize * vec2(0.0, 0.0), compare, bias);
    float lt = texture2DCompare(depths, centroidUV + texelSize * vec2(0.0, 1.0), compare, bias);
    float rb = texture2DCompare(depths, centroidUV + texelSize * vec2(1.0, 0.0), compare, bias);
    float rt = texture2DCompare(depths, centroidUV + texelSize * vec2(1.0, 1.0), compare, bias);
    float a = mix(lb, lt, f.y);
    float b = mix(rb, rt, f.y);
    float c = mix(a, b, f.x);
    return c;
}

float PCFLerp(sampler2D depths, vec2 uv, float compare, float bias) 
{
    vec2 texelSize = vec2(1.0) / vec2(2048.0, 2048.0);
    float result = 0.0;
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            vec2 off = texelSize * vec2(x,y);
            result += texture2DShadowLerp(depths, uv + off, compare, bias);
        }
    }
    return result / 9.0;
}

// 使用PCF加双线性滤波
float calcShadow(sampler2D depths, vec4 FragPosLightSpace, vec3 lightDir, vec3 normal) 
{
    // Clipped Coord -> NDC -> texture Coord
    vec3 projectCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projectCoords = projectCoords * 0.5 + 0.5;
    
    if(projectCoords.z > 1.0)
        return 0;
    
    // 获取 shadow map 中保存的深度值
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    return PCFLerp(depths,projectCoords.xy,projectCoords.z,bias);
}


float ShadowCaculation(vec4 FragPosLightSpace)
{
    vec3 projectCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projectCoords = projectCoords * 0.5 + 0.5;

    if(projectCoords.z > 1.0)
    {return 0;}

    float closestDepth = texture(shadowMap, projectCoords.xy).r;
    float currentDepth = projectCoords.z;
    //float bias = 0.005;
    float bias = max(0.05 * (1.0 - dot(fs_in.Normal, normalize(lightPos - fs_in.FragPos))), 0.005);
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // percentage-closer filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projectCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}







void main()
{
    vec3 color = texture(diffuseTex, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    // Ambient
    vec3 ambient = 0.15 * color;
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor * color;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

    vec3 specular = spec * lightColor * vec3(texture(specularTex, fs_in.TexCoords));  
    //vec3 specular = spec * lightColor;  


    // Shadow
    //float shadow = ShadowCaculation(fs_in.FragPosLightSpace);
    float shadow = calcShadow(shadowMap, fs_in.FragPosLightSpace, lightDir, normal);
    vec3 lighting = ambient + ((1.0 - shadow) * (diffuse + specular));

    // // HDR tonemapping
    // lighting = lighting / (lighting + vec3(1.0));
    // // gamma correct
    // lighting = pow(lighting, vec3(1.0/2.2)); 

    FragColor = vec4(lighting, 1.0f);
}

