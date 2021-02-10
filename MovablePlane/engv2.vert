#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexTangent;
in vec2 vertexTexCoord;

out vec3 worldPosition1;
out vec3 worldNormal1;
out vec4 worldTangent1;
out vec2 texCoord1;
out float ClipDistance;
out vec4 vCoord;
out vec4 normal;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;
uniform vec4 plane;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform float texCoordScale;

void main()
{
    // Pass through scaled texture coordinates
    texCoord1 = vertexTexCoord * texCoordScale;

    // Transform position, normal, and tangent to world space
    worldPosition1 = vec3(modelMatrix * vec4(vertexPosition, 1.f));
    worldNormal1 = normalize(modelNormalMatrix * vertexNormal);
    worldTangent1.xyz = normalize(vec3(modelMatrix * vec4(vertexTangent.xyz, 0.f)));
    worldTangent1.w = vertexTangent.w;

    vCoord = modelMatrix * vec4(vertexPosition, 1.0f);

    // Calculate vertex position in clip coordinates
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.f);
    ClipDistance = dot(modelMatrix * vec4(vertexPosition, 1.f), plane);
    if (plane.xyz == vec3(.0f, .0f, .0f))
        normal = vec4(.0f, .0f, .0f, .0f);
    else
        normal = (projectionMatrix * viewMatrix * vec4(normalize(plane.xyz), 1.f)) -
                 (projectionMatrix * viewMatrix * vec4(0, 0, 0, 1));
}
