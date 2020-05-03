/*
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
void main()
{
   FragColor = vec4(ourColor, 1.0f);
}
//*/

//*
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec3 vertexPos;
void main()
{
   FragColor = vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0f);
}
//*/