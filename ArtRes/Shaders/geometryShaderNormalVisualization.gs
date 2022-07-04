#version 330 core
layout (triangles) in;
// 其实本来在C++里这个模型是glDrawElements(GL_TRIANGLES, ......)，绘制三角形的，所以这里的in是triangles，但是
// 在下一行定义了输出为line_strip，相当于在几何着色器里把绘制方式改了，类似glDrawElements(GL_LINES, ......)
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();
    // 因为这里只用EndPrimitive生成了线段，没有生成原本绘制模型要的的三角形，所以绘制出来就只有法线了
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal

    /* 启用这里的代码再把max_vertices改成12就可以把原本模型的轮廓勾勒出来
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
    */
}