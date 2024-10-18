#version 330

// Input fragment attributes (from fragment shader)
in vec2 fragTexCoord;
out vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;

void main()
{
    fragColor = texture(texture0, fragTexCoord);
}
