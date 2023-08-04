//
// Created by bhavya on 2023-07-28.
//
#include <glm/glm.hpp>

using namespace glm;
using namespace std;

struct cubeVertex {
    cubeVertex(vec3 _position, vec4 _color, vec2 _uv, vec3 _normal)
            : position(_position), color(_color), uv(_uv), normal(_normal) {}

    vec3 position;
    vec4 color;
    vec2 uv;
    vec3 normal;
};

// Textured Cube model
const cubeVertex texturedCubeVertexArray[] = {  // position, color, uv, normal
        // Left face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),

        // Top face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),

        // Front face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),

        // Bottom face
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),

        // Right face
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),

        // Back face
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f))
};

// Transparent Textured Cube model
const cubeVertex frontTransparentTexturedCubeVertexArray[] = {  // position, color, uv, normal
        // Left face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),

        // Top face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),

        // Front face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),

        // Bottom face
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),

        // Right face
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),

        // Back face
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f))
};

// Transparent Textured Cube model
const cubeVertex transparentTexturedCubeVertexArray[] = {  // position, color, uv, normal
        // Left face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 0.3f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 0.3f), vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 0.3f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 0.0f, 0.3f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 0.3f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 0.0f, 0.0f, 0.3f), vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)),

        // Top face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 0.3f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(0.0f, 0.0f, 1.0f, 0.3f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(0.0f, 0.0f, 1.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),

        // Front face
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 1.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),

        // Bottom face
        cubeVertex(vec3(-0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 0.3f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 0.3f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(0.0f, 1.0f, 0.0f, 0.3f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(0.0f, 1.0f, 0.0f, 0.3f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)),

        // Right face
        cubeVertex(vec3(0.5f, -0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 0.3f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 0.3f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 0.3f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 0.3f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(1.0f, 0.0f, 1.0f, 0.3f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)),
        cubeVertex(vec3(0.5f, 0.5f, 0.5f), vec4(1.0f, 0.0f, 1.0f, 0.3f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),

        // Back face
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 0.3f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(0.5f, -0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 0.3f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 0.3f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)),
        cubeVertex(vec3(-0.5f, 0.5f, -0.5f), vec4(1.0f, 1.0f, 0.0f, 0.3f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f))
};

/**
 * Creates a Vertex Array Object that contains a cube with texture coordinates
 */
int createCubeVAO() {
    // Create a vertex array
    GLuint vertexArrayObject, vertexBufferObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          sizeof(cubeVertex), // stride - each vertex contain 2 vec3 (position, color)
                          (void *) offsetof(cubeVertex, position)             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, color)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, uv)    // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,                            // attribute 2 matches aUV in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, normal)     // normal is offseted by 3 vec3 (comes after position, color and uv)
    );
    glEnableVertexAttribArray(3);

    return vertexArrayObject;
}

/**
 * Creates a Vertex Array Object that contains a cube with front transparent texture coordinates
 */
int createFrontTransparentCubeVAO() {
    // Create a vertex array
    GLuint vertexArrayObject, vertexBufferObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frontTransparentTexturedCubeVertexArray), frontTransparentTexturedCubeVertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          sizeof(cubeVertex), // stride - each vertex contain 2 vec3 (position, color)
                          (void *) offsetof(cubeVertex, position)             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, color)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, uv)    // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,                            // attribute 2 matches aUV in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, normal)     // normal is offseted by 3 vec3 (comes after position, color and uv)
    );
    glEnableVertexAttribArray(3);

    return vertexArrayObject;
}

/**
 * Creates a Vertex Array Object that contains a cube with transparent texture coordinates
 */
int createTransparentCubeVAO() {
    // Create a vertex array
    GLuint vertexArrayObject, vertexBufferObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentTexturedCubeVertexArray), transparentTexturedCubeVertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          sizeof(cubeVertex), // stride - each vertex contain 2 vec3 (position, color)
                          (void *) offsetof(cubeVertex, position)             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, color)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, uv)    // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,                            // attribute 2 matches aUV in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(cubeVertex),
                          (void *) offsetof(cubeVertex, normal)     // normal is offseted by 3 vec3 (comes after position, color and uv)
    );
    glEnableVertexAttribArray(3);

    return vertexArrayObject;
}