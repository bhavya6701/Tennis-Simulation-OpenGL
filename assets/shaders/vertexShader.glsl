#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);
uniform mat4 lightSpaceMatrix;

out vec2 vertexUV;
out vec3 fragmentNormal;
out vec3 fragmentPosition;
out vec4 fragPosLightSpace;

void main() {
    vertexUV = aUV;
    fragmentNormal = mat3(worldMatrix) * aNormal;
    fragmentPosition = vec3(worldMatrix * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrix * vec4(fragmentPosition, 1.0f);

    gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}