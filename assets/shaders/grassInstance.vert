#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;
layout (location = 4) in mat4 aInstanceMatrix;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;
out vec2 worldXZ;

uniform float time;

uniform float windScale;
uniform vec3 windDirection;
uniform float phaseScale;

void main()
{
    vec4 transformPosition = vec4(aPos, 1.0);

    // vec4 vertice world position
    transformPosition = modelMatrix * aInstanceMatrix * transformPosition;
    worldXZ = transformPosition.xz;

    // Wind
    vec3 windDirN = normalize(windDirection);
    float phaseDistance = dot(windDirN, transformPosition.xyz);
    transformPosition += vec4(sin(time + phaseDistance / phaseScale) * (1.0 - aColor.r) * windScale * windDirN, 0);

    // vec3 vertice world position for fragment light calculation
    worldPosition = transformPosition.xyz;

    gl_Position = projectionMatrix * viewMatrix * transformPosition;

    uv = aUV;

    normal = transpose(inverse(mat3(modelMatrix * aInstanceMatrix))) * aNormal;
}