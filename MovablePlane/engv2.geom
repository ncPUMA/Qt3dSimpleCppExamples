#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 worldPosition1[];
in vec3 worldNormal1[];
in vec4 worldTangent1[];
in vec2 texCoord1[];
in vec4 vCoord[];
in float ClipDistance[];
in vec4 normal[];

out vec3 worldPosition;
out vec3 worldNormal;
out vec4 worldTangent;
out vec2 texCoord;

uniform vec4 plane;

void main()
{
    worldPosition = worldPosition1[1];
    worldNormal = worldNormal1[1];
    worldTangent = worldTangent1[1];
    texCoord = texCoord1[1];

    int countInto = 0;
    for(int i = 0; i < vCoord.length(); i++)
        if (ClipDistance[i] < 0.f)
            ++countInto;

    vec4 norm = normal[0] / 5.f;
    if (countInto > 0)
        norm *= -1.f;

    gl_Position = gl_in[0].gl_Position + norm;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + norm;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + norm;
    EmitVertex();

    EndPrimitive();
}
