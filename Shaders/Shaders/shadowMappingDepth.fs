#version 330 core

void main()
{
    // 下面这行代码注释与否都可以，只不过不注释的话相当于显示的填充深度值，效率不如注释掉，因为OpenGL本来就会填充
    // gl_FragDepth = gl_FragCoord.z;
}