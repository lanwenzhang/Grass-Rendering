#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aInstanceMatrix;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

uniform float time;

void main()
{
    vec4 transformPosition = vec4(aPos, 1.0);

    // vec4 vertice world position
    transformPosition = modelMatrix * aInstanceMatrix * transformPosition;

    // vec3 vertice world position for fragment light calculation
    worldPosition = transformPosition.xyz;

    gl_Position = projectionMatrix * viewMatrix * transformPosition;

    uv = aUV;

    normal = normalMatrix * aNormal;
}