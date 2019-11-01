#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TexCoord);
    // float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    // 人眼会对绿色更加敏感一些，而对蓝色不那么敏感，所以为了获取物理上更精确的效果，我们需要使用加权的(Weighted)通道
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}

