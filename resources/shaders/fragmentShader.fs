#version 330 core
out vec4 FragColor;

in vec2 outTexCords;

uniform sampler2D t0;
uniform sampler2D t1;

uniform float p;

void main()
{
    FragColor = mix(texture(t0, outTexCords), texture(t1, outTexCords), 0.2);
}