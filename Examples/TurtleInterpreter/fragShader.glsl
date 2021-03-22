#version 330 core

in vec3 vcolor;
out vec4 fragColor;

void main()
{
//    float a = 1 - vcolor.g;// / 2;
    fragColor = vec4(vcolor,1);
}