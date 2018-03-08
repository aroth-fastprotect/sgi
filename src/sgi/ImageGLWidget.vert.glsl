#version 130
in vec3 position;
in vec2 texCoord;
out vec2 oe_sg_texcoord0;
void main()
{
    gl_Position = vec4(position, 1.0);
    $GLSL_COLOR_FILTER_VERTEX
    oe_sg_texcoord0 = texCoord;
}


