/*
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
void main()
{
   gl_Position = vec4(aPos, 1.0);
   ourColor = aColor;
}
//*/

/*
// Exercise 1
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
void main()
{
   gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0);
   ourColor = aColor;
}
//*/

//*
// Exercise 3
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
out vec3 vertexPos;
void main()
{
   gl_Position = vec4(aPos, 1.0);
   ourColor = aColor;
   vertexPos = aPos;
}
//*/