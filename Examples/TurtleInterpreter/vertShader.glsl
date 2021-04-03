#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 vcolor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);//vec4(position, 1.0);//
    //float Z = gl_Position.z;
    //1/x2
//    Z = 1 / (Z*Z);
//    Z = clamp(Z,0,1);
    float Z = (gl_Position.z / gl_Position.w);// * 3.14159;/2.0;
//    Z = 1.2 * sin(sin(sqrt(1-Z)));
    //Z = sin(sqrt(1-Z));
    Z = 2 - (atan(Z) + 1);
//    Z = sqrt(Z);
    //Z = sin(Z);
    ////Z = 1.2 * sin(1 - atan(Z-0.2)) - 0.2;
    //vcolor = vec3(1,1,1);//color;//vec3(color.r * Z, color.g * Z, color.b);//color *Z;
    vcolor = vec3(color.r*Z, color.g *Z, color.b*Z);
}