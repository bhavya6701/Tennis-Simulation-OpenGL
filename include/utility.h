#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "GL/glew.h"
#include <stb_image.h>

#define STB_IMAGE_IMPLEMENTATION

using namespace glm;
using namespace std;

/**
 * Sets a uniform vec4 variable in the shader
 */
void SetUniformMat4(GLuint shader_id, const char *uniform_name, mat4 uniform_value) {
    glUseProgram(shader_id);
    glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

/**
 * Sets a uniform vec4 variable in the shader
 */
template<class T>
void SetUniform1Value(GLuint shader_id, const char *uniform_name, T uniform_value) {
    glUseProgram(shader_id);
    glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
    glUseProgram(0);
}

/**
 * Sets a uniform vec3 variable in the shader
 */
void SetUniformVec3(GLuint shader_id, const char *uniform_name, vec3 uniform_value) {
    glUseProgram(shader_id);
    glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, &uniform_value[0]);
}

/**
 * Load object file and set up vertex, UV, and normal buffers
 */
GLuint setupModelVBO(string path, int &vertexCount) {
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<vec2> UVs;

    //read the vertex data from the model's OBJ file
    loadOBJ(path.c_str(), vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(0);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(1);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
    vertexCount = vertices.size();
    return VAO;
}

/**
 * Load object file and set up vertex, UV, normal and element buffers
 */
GLuint setupModelEBO(string path, int &vertexCount) {
    vector<int> vertexIndices;
    //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<vec2> UVs;

    //read the vertices from the cube.obj file
    //We won't be needing the normals or UVs for this program
    loadOBJ2(path.c_str(), vertexIndices, vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(0);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(1);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(2);

    //EBO setup
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int), &vertexIndices.front(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    vertexCount = vertexIndices.size();
    return VAO;
}

/**
 * Initialize the frame buffer for the shadow map
 */
void initializeFrameBuffer(GLuint depthMapFBO, int DEPTH_MAP_TEXTURE_SIZE) {
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE,
                 DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
    glDrawBuffer(GL_NONE);
}

/**
 * Loads a texture from a file and returns the texture ID
 */
GLuint loadTexture(const char *filename) {
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLint format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

/**
 * Checks if the ball intersects the ground
 */
bool intersectsGround(vec3 planePoint, vec3 planeNormal, vec3 ballPosition, float ballRadius) {
    return dot(planeNormal, ballPosition - planePoint) < ballRadius;
}

/**
 * Checks if the ball intersects the racket
 */
bool intersectsRacket(const vec3 &racketPosition, const vec3 &racketNormal, float racketWidth, float racketHeight,
                      vec3 ballPosition, float ballRadius) {
    vec3 ballToRacket = racketPosition - ballPosition;
    float distance = dot(ballToRacket, racketNormal);

    if (abs(distance) <= ballRadius) {
        vec3 projectedPoint = ballPosition + distance * racketNormal;
        vec3 closestPointOnRacket = clamp(projectedPoint, racketPosition - racketWidth / 2,
                                          racketPosition + racketWidth / 2);

        // Check if the projected point is within the width and height of the racket
        bool withinWidthRange = (closestPointOnRacket.x >= racketPosition.x - racketWidth / 2 &&
                                 closestPointOnRacket.x <= racketPosition.x + racketWidth / 2);
        bool withinHeightRange = (closestPointOnRacket.y >= racketPosition.y - racketHeight / 2 &&
                                  closestPointOnRacket.y <= racketPosition.y + racketHeight / 2);

        if (withinWidthRange && withinHeightRange) {
            float distanceToClosestPoint = length(projectedPoint - closestPointOnRacket);
            if (distanceToClosestPoint <= ballRadius) {
                return true; // Collision detected within the racket's width and height
            }
        }
    }
    return false; // No collision
}

/**
 * Checks if the ball intersects the net
 */
bool intersectsNet(const vec3 &netPosition, const vec3 &netNormal, float netWidth, float netHeight, vec3 ballPosition,
                   float ballRadius) {
    vec3 ballToNet = netPosition - ballPosition;
    float distance = dot(ballToNet, netNormal);

    if (abs(distance) <= ballRadius) {
        vec3 projectedPoint = ballPosition + distance * netNormal;
        vec3 closestPointOnNet = projectedPoint; // Assuming the net is a plane, just use the projected point

        // Check if the projected point is within the width and height of the net on either side
        float halfWidth = netWidth / 2;
        float halfHeight = netHeight / 2;

        bool withinWidthRange = (closestPointOnNet.x >= netPosition.x - halfWidth &&
                                 closestPointOnNet.x <= netPosition.x + halfWidth);
        bool withinHeightRange = (closestPointOnNet.y >= netPosition.y - halfHeight &&
                                  closestPointOnNet.y <= netPosition.y + halfHeight);

        if (withinWidthRange && withinHeightRange) {
            float distanceToClosestPoint = length(projectedPoint - closestPointOnNet);
            if (distanceToClosestPoint <= ballRadius) {
                return true; // Collision detected within the net's width and height
            }
        }
    }
    return false; // No collision
}