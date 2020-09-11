#version 330 core

out vec4 FragColor;
uniform vec3 rectColor;

void main()
{
   FragColor = vec4(rectColor / 256.0, 1.0);
}
