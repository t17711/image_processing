#version 410

uniform sampler2D texture;

out vec2 v_texcoord;

void main()
{
    // Set fragment color from texture
    gl_FragColor = texture2D(texture, v_texcoord);
}