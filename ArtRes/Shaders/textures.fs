/*
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}
//*/

/*
// Exercise 1
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec2 TexCoord2 = vec2(1-TexCoord.x, TexCoord.y);
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord2), 0.2);
}
//*/

/*
// Exercise 2
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord*2), 0.2);
}
//*/

/*
// Exercise 3
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec2 zoomTexC = TexCoord / 20;
	FragColor = mix(texture(texture1, zoomTexC), texture(texture2, zoomTexC), 0.2);
}
//*/

//*
// Exercise 4
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

// mix factor
uniform float mixFactor;

void main()
{
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixFactor);
}
//*/