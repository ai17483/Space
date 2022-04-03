#version 330 core
out vec4 FragColor;
out vec4 Texture1;

in vec2 outTexCords;

uniform sampler2D t0;
uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor, 1.0f);
    //Texture1 = texture(t0, outTexCords);

}