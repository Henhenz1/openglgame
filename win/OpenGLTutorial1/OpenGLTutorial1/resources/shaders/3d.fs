#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// texture samplers
uniform sampler2D texture1;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    FragColor = texture(texture1, TexCoords);
}
