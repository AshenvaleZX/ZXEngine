#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

/*
// 正常渲染
void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
} 
//*/

/*
// 反色
void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(vec3(1 - col), 1.0);
} 
//*/

/*
// 灰度
void main()
{
    FragColor = texture(screenTexture, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
} 
//*/

//*
// 通过核(Kernel)或者说卷积矩阵(Convolution Matrix)渲染的一些效果
const float offset = 1.0 / 300.0;  
void main()
{
    // 这里用3x3矩阵，所以9个偏移量
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    // 锐化核
    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );

    // 模糊核
    // float kernel[9] = float[](
    // 1.0 / 16, 2.0 / 16, 1.0 / 16,
    // 2.0 / 16, 4.0 / 16, 2.0 / 16,
    // 1.0 / 16, 2.0 / 16, 1.0 / 16  
    // );

    // 边缘检测核
    float kernel[9] = float[](
        1,  1, 1,
        1, -8, 1,
        1,  1, 1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
}
//*/