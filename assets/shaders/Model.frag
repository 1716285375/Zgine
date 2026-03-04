#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 v_TexCoords;
in vec4 v_Color;

uniform sampler2D u_Texture0;
uniform bool u_HasTexture;
uniform vec4 u_BaseColor;

void main()
{
    vec4 base = u_BaseColor;
    if (u_HasTexture) {
        base *= texture(u_Texture0, v_TexCoords);
    }
    color = base * v_Color;
}

