/**
 * COMP371: Computer Graphics |
 * Final Project |
 * Name: Bhavya Ruparelia | Devansh Vaidya | Jananee Aruboribaran |
 * Student ID: 40164863 | 40165987 | 40129224 |
 */

#include <iostream>
#include <vector>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL

#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context, initializing
// OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include "shader.h"
#include "OBJloader.h"  //For loading .obj files
#include "OBJloaderV2.h"  //For loading .obj files using a polygon list format

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

using namespace glm;
using namespace std;

// Window for rendering
GLFWwindow *window;

// Shaders
GLuint depthShaderProgram, colorShaderProgram;

// Frame buffer and shadow map width and height
int frameBufferWidth, frameBufferHeight;
int DEPTH_MAP_TEXTURE_SIZE = 1024;

// Frame buffer object for shadow mapping
GLuint depthMapFBO;

// Textures for court objects
GLuint tennisCourtGrassTextureID, tennisCourtLineTextureID, blueBoxTextureID, skin1TextureID, skin2TextureID,
        skin3TextureID, skin4TextureID, racketHandleTextureID, racketSideTextureID, racketNetTextureID, ballTextureID,
        whiteTextureID, silverPoleTextureID, tennisNetTextureID;
GLuint playerTitleTextureID[2];

// Ground point
vec3 groundPoint(0.0f, -1.0f, 0.0f);

// Vertices counts and VAO for textured cube and sphere
int cubeVerticesCount, sphereVerticesCount;
GLuint cubeVAO, sphereVAO;

// Vertices count and VAO for world
int itemsVertices[7];
GLuint itemsVAO[7];
GLuint worldTextures[9], scoreBoardTextures[10];

// Matrix declarations
mat4 worldMatrix, viewMatrix, projectionMatrix, rotationMatrix, translationMatrix, scalingMatrix,
        modelHierarchicalMatrix, groundTennisHierarchicalMatrix;

// Tilting parameters for camera
float dt = 0.0f, cameraVerticalAngle = 0.0f;

// Camera constants
vec3 cameraUp(0.0f, 1.0f, 0.0f),
        cameraSideVector = vec3(1.0f, 1.0f, 1.0f);
float cameraRotationSpeed = 50.0f;

// Camera parameters for view transform
vec3 cameraPosition[] = {vec3(0.0f, 20.0f, 70.0f),
                         vec3(-5.0f, 2.25f, -20.0f),
                         vec3(-5.0f, 2.25f, 20.0f),
                         vec3(0.0f, 7.5f, 60.0f)},
        cameraLookAt = cameraPosition[0];
int cameraIndex = 0;
float cameraRotationAngle = 0.0f,
        cameraRadius = length(cameraPosition[0]),
        cameraTheta = atan(cameraPosition[0].z, cameraPosition[0].x),
        cameraPhi = asin(cameraPosition[0].y / cameraRadius),
        cameraPosX, cameraPosY, cameraPosZ;

// Mouse parameters
bool isLeftMBClicked = false, isRightMBClicked = false, isMiddleMBClicked = false;
double lastMousePosX = 0.0f, lastMousePosY = 0.0f;

float lastBtnClick = 0.0f;

// Time frame parameter
float lastFrameTime = (float) glfwGetTime();

// Model constants
float rotateByAngle = 5.0f, otherRotateByAngle = 0.5f;
mat4 defaultArmRotationMatrix[] = {
        rotate(mat4(1.0f), radians(-60.0f), vec3(0.0f, 0.0f, 1.0f)),
        rotate(mat4(1.0f), radians(60.0f), vec3(0.0f, 0.0f, 1.0f))
},
        defaultArmTranslationMatrix[] = {
        translate(mat4(1.0f), vec3(0.5f, 3.3f, 0.42f)),
        translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.0f))
};

// Model parameters for model transform
vec3 modelPosition[] = {vec3(0.0f, 0.25f, 40.0f),
                        vec3(-5.0f, 0.25f, -40.0f)};

float yRotationAngle[] = {0.0f, 0.0f};
mat4 yRotationMatrix[] = {rotate(mat4(1.0f), yRotationAngle[0], vec3(0.0f, 1.0f, 0.0f)),
                          rotate(mat4(1.0f), yRotationAngle[1], vec3(0.0f, 1.0f, 0.0f))};

float armXRotationAngle[2][3] = {{0.0f, 0.0f, 0.0f},
                                 {0.0f, 0.0f, 0.0f}},
        armYRotationAngle[2][3] = {{0.0f, 0.0f, 0.0f},
                                   {0.0f, 0.0f, 0.0f}},
        armZRotationAngle[2][3] = {{0.0f, 0.0f, 0.0f},
                                   {0.0f, 0.0f, 0.0f}};
mat4 armXRotationMatrix[2][3] = {{rotate(mat4(1.0f), armXRotationAngle[0][0], vec3(1.0f, 0.0f, 0.0f)),
                                         rotate(mat4(1.0f), armXRotationAngle[0][1], vec3(1.0f, 0.0f, 0.0f)),
                                         rotate(mat4(1.0f), armXRotationAngle[0][2], vec3(1.0f, 0.0f, 0.0f))},
                                 {rotate(mat4(1.0f), armXRotationAngle[1][0], vec3(1.0f, 0.0f, 0.0f)),
                                         rotate(mat4(1.0f), armXRotationAngle[1][1], vec3(1.0f, 0.0f, 0.0f)),
                                         rotate(mat4(1.0f), armXRotationAngle[1][2], vec3(1.0f, 0.0f, 0.0f))}},
        armYRotationMatrix[2][3] = {{rotate(mat4(1.0f), armYRotationAngle[0][0], vec3(0.0f, 1.0f, 0.0f)),
                                            rotate(mat4(1.0f), armYRotationAngle[0][1], vec3(0.0f, 1.0f, 0.0f)),
                                            rotate(mat4(1.0f), armYRotationAngle[0][2], vec3(0.0f, 1.0f, 0.0f))},
                                    {rotate(mat4(1.0f), armYRotationAngle[1][0], vec3(0.0f, 1.0f, 0.0f)),
                                            rotate(mat4(1.0f), armYRotationAngle[1][1], vec3(0.0f, 1.0f, 0.0f)),
                                            rotate(mat4(1.0f), armYRotationAngle[1][2], vec3(0.0f, 1.0f, 0.0f))}},
        armZRotationMatrix[2][3] = {{rotate(mat4(1.0f), armZRotationAngle[0][0], vec3(0.0f, 0.0f, 1.0f)),
                                            rotate(mat4(1.0f), armZRotationAngle[0][1], vec3(0.0f, 0.0f, 1.0f)),
                                            rotate(mat4(1.0f), armZRotationAngle[0][2], vec3(0.0f, 0.0f, 1.0f))},
                                    {rotate(mat4(1.0f), armZRotationAngle[1][0], vec3(0.0f, 0.0f, 1.0f)),
                                            rotate(mat4(1.0f), armZRotationAngle[1][1], vec3(0.0f, 0.0f, 1.0f)),
                                            rotate(mat4(1.0f), armZRotationAngle[1][2], vec3(0.0f, 0.0f, 1.0f))}},
        armRotationMatrix[2][3] = {{armXRotationMatrix[0][0] * armYRotationMatrix[0][0] * armZRotationMatrix[0][0],
                                           armXRotationMatrix[0][1] * armYRotationMatrix[0][1] *
                                           armZRotationMatrix[0][1],
                                           armXRotationMatrix[0][2] * armYRotationMatrix[0][2] *
                                           armZRotationMatrix[0][2]},
                                   {armXRotationMatrix[1][0] * armYRotationMatrix[1][0] * armZRotationMatrix[1][0],
                                           armXRotationMatrix[1][1] * armYRotationMatrix[1][1] *
                                           armZRotationMatrix[1][1],
                                           armXRotationMatrix[1][2] * armYRotationMatrix[1][2] *
                                           armZRotationMatrix[1][2]}};
int selectedFigure = 0, selectedModel = 0;

// Ball and Racket Constants for collision
vec3 ballScaling(0.25f, 0.25f, 0.25f),
        ballVelocity(0.0f, 0.0f, 0.0f),
        tennisScaling(48.0f, 4.14f, 0.5f),
        groundNormal(0.0f, 1.0f, 0.0f),
        racketNetScaling(1.33f, 1.66f, 0.16f);
float ballRadius = ballScaling.x;
const float GRAVITY = 9.807f, DAMPING = 1.0f;

// Ball bounce parameters
vec3 ballPosition(5.0f, 20.0f, 35.0f),
        ballRotationAxis(0.0f, 1.0f, 0.0f),
        ballAngularAxis(0.0f, 1.0f, 0.0f),
        tennisNetPosition(0.0f, 2.12f, 0.0f),
        tennisNetNormal(0.0f, 0.0f, 1.0f),
        racket1Position, racket2Position, racket1Normal, racket2Normal;
float ballRotationAngle = 0.0f, ballAngularVelocity = 0.0f;
mat4 racketNetTransformation[] = {rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 1.0f, 0.0f)),
                                  rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 1.0f, 0.0f))};

// Required for score calculation
int nextServe = 1, bounce = 0, lastBounceRacket = 0, p1Score = 0, p2Score = 0;

// Light parameters
vec3 lightPosition;
mat4 lightProjectionMatrix;
bool isSpotLightOn = false, isShadowOn = false;

// Check if the ball touches the ground
bool intersectsGround(vec3 planePoint, vec3 planeNormal) {
    return dot(planeNormal, ballPosition - planePoint) < ballRadius;
}

bool intersectsRacket(const vec3 &racketPosition, const vec3 &racketNormal, float racketWidth, float racketHeight) {
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

// Check if the ball touches the net
bool intersectsNet(const vec3 &netPosition, const vec3 &netNormal, float netWidth, float netHeight) {
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

void resetHome() {
    cameraPosition[0] = vec3(0.0f, 20.0f, 70.0f);
    cameraPosition[1] = vec3(-5.0f, 2.25f, -20.0f);
    cameraPosition[2] = vec3(-5.0f, 2.25f, 20.0f);
    cameraLookAt = -cameraPosition[0];
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    cameraIndex = 0;
    modelPosition[0] = vec3(0.0f, 0.25f, 40.0f), modelPosition[1] = vec3(-5.0f, 0.25f, -40.0f);
    yRotationAngle[0] /= -2, yRotationAngle[1] /= -2;
    armXRotationAngle[0][0] /= -2, armYRotationAngle[0][0] /= -2, armZRotationAngle[0][0] /= -2;
    armXRotationAngle[0][1] /= -2, armYRotationAngle[0][1] /= -2, armZRotationAngle[0][1] /= -2;
    armXRotationAngle[0][2] /= -2, armYRotationAngle[0][2] /= -2, armZRotationAngle[0][2] /= -2;
    armXRotationAngle[1][0] /= -2, armYRotationAngle[1][0] /= -2, armZRotationAngle[1][0] /= -2;
    armXRotationAngle[1][1] /= -2, armYRotationAngle[1][1] /= -2, armZRotationAngle[1][1] /= -2;
    armXRotationAngle[1][2] /= -2, armYRotationAngle[1][2] /= -2, armZRotationAngle[1][2] /= -2;
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
    GLenum format = 0;
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
 * Mouse button callback
 */
void mouseBtnCallback(GLFWwindow *handle, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        isLeftMBClicked = true;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        isLeftMBClicked = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        isRightMBClicked = true;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        isRightMBClicked = false;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        isMiddleMBClicked = true;
    } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
        isMiddleMBClicked = false;
    }
}

/**
 * Cursor position callback
 */
void cursorCallback(GLFWwindow *handle, double xPos, double yPos) {
    double deltaX = lastMousePosX - xPos;
    double deltaY = lastMousePosY - yPos;

    // Zoom in/out of the scene
    if (isLeftMBClicked) {
        if (deltaY > 0) {
            cameraPosition[0] -= vec3(0.0f, 0.0f, 0.5f);
        } else {
            cameraPosition[0] += vec3(0.0f, 0.0f, 0.5f);
        }
    }

    // Move camera horizontally
    if (isRightMBClicked) {
        if (deltaX > 0) {
            cameraPosition[0] -= vec3(0.5f, 0.0f, 0.0f);
        } else {
            cameraPosition[0] += vec3(0.5f, 0.0f, 0.0f);
        }
    }

    // Tilt camera vertically
    if (isMiddleMBClicked) {
        const float cameraAngularSpeed = 60.0f;
        cameraVerticalAngle -= deltaY * cameraAngularSpeed * dt;
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));

        float theta = radians(90.0f);
        float phi = radians(cameraVerticalAngle);

        cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
        cameraSideVector = normalize(cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f)));
        cameraPosition[0] += cameraSideVector * dt;
    }
}

/**
 * Handle inputs from keyboard
 */
void handleInputs() {
    // Poll for and process events
    glfwPollEvents();

    // Exit program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Move model to +z direction
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (cameraIndex == 2) {
            modelPosition[selectedModel].z =
                    modelPosition[selectedModel].z < 54.17f ? modelPosition[selectedModel].z + 0.125f : 54.17f;
        } else {
            modelPosition[selectedModel].z =
                    modelPosition[selectedModel].z > -54.17f ? modelPosition[selectedModel].z - 0.125f : -54.17f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (cameraIndex == 2) {
            modelPosition[selectedModel].z =
                    modelPosition[selectedModel].z > -54.17f ? modelPosition[selectedModel].z - 0.125f : -54.17f;
        } else {
            modelPosition[selectedModel].z =
                    modelPosition[selectedModel].z < 54.17f ? modelPosition[selectedModel].z + 0.125f : 54.17f;
        }
    }

    // Rotate model to the left around y-axis by 5 degrees
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK)) {
            // 5 degrees = 0.0872665 radians
            yRotationAngle[selectedModel] -= radians(rotateByAngle);
        } else {
            if (cameraIndex == 2) {
                modelPosition[selectedModel].x =
                        modelPosition[selectedModel].x < 25.0f ? modelPosition[selectedModel].x + 0.125f : 25.0f;
            } else {
                modelPosition[selectedModel].x =
                        modelPosition[selectedModel].x > -25.0f ? modelPosition[selectedModel].x - 0.125f : -25.0f;
            }
        }
    }

    // Rotate model to the right around y-axis by 5 degrees
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK)) {
            yRotationAngle[selectedModel] += radians(rotateByAngle);
        } else {
            if (cameraIndex == 2) {
                modelPosition[selectedModel].x =
                        modelPosition[selectedModel].x > -25.0f ? modelPosition[selectedModel].x - 0.25f : -25.0f;
            } else {
                modelPosition[selectedModel].x =
                        modelPosition[selectedModel].x < 25.0f ? modelPosition[selectedModel].x + 0.25f : 25.0f;
            }
        }
    }

    // Select 1st model
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        selectedModel = 0;
    }

    // Select 2nd model
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        selectedModel = 1;
    }

    // Select part of the arm
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            selectedFigure = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            selectedFigure = 1;
        }
    }

    // Rotate arm in x-axis
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        armXRotationAngle[selectedModel][selectedFigure] -= radians(otherRotateByAngle);
    }

    // Rotate arm in x-axis
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        armXRotationAngle[selectedModel][selectedFigure] += radians(otherRotateByAngle);
    }

    // Rotate arm in y-axis
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        armYRotationAngle[selectedModel][selectedFigure] -= radians(otherRotateByAngle);
    }

    // Rotate arm in y-axis
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        armYRotationAngle[selectedModel][selectedFigure] += radians(otherRotateByAngle);
    }

    // Rotate arm in z-axis
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        armZRotationAngle[selectedModel][selectedFigure] -= radians(otherRotateByAngle);
    }

    // Rotate arm in z-axis
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        armZRotationAngle[selectedModel][selectedFigure] += radians(otherRotateByAngle);
    }

    // Reset camera position
    if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) {
        resetHome();
    }

    // Turn spotlight on/off
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        float differentTime = glfwGetTime() - lastBtnClick;
        if (differentTime > 1.0f) {
            lastBtnClick = glfwGetTime();
            SetUniform1Value(colorShaderProgram, "isSpotLightOn", isSpotLightOn ? 0 : 1);
            isSpotLightOn = !isSpotLightOn;
        }
    }

    // Turn shadow on/off
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        float differentTime = glfwGetTime() - lastBtnClick;
        if (differentTime > 1.0f) {
            lastBtnClick = glfwGetTime();
            SetUniform1Value(colorShaderProgram, "isShadowOn", isShadowOn ? 0 : 1);
            isShadowOn = !isShadowOn;
        }
    }

    // Switch between cameras
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        float differentTime = glfwGetTime() - lastBtnClick;
        if (differentTime > 1.0f) {
            lastBtnClick = glfwGetTime();
            cameraIndex = cameraIndex == 0 ? 1 : cameraIndex == 1 ? 2 : 0;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        cameraIndex = 3;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        cameraIndex = 0;
    }

    // Camera Rotation - Up, Down, Left, Right
    if (cameraIndex == 0 &&
        (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
         glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
        cameraRotationAngle = cameraRotationSpeed * dt;
        cameraRadius = length(cameraPosition[0]);
        cameraTheta = atan(cameraPosition[0].z, cameraPosition[0].x);
        cameraPhi = asin(cameraPosition[0].y / cameraRadius);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            cameraPhi += radians(cameraRotationAngle);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPhi -= radians(cameraRotationAngle);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            cameraTheta += radians(cameraRotationAngle);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            cameraTheta -= radians(cameraRotationAngle);
        }
        cameraPhi = clamp(cameraPhi, -pi<float>() / 2.0f + 0.01f, pi<float>() / 2.0f - 0.01f);
        cameraPosX = cameraRadius * cos(cameraPhi) * cos(cameraTheta);
        cameraPosY = cameraRadius * sin(cameraPhi);
        cameraPosZ = cameraRadius * cos(cameraPhi) * sin(cameraTheta);
        cameraPosition[0] = vec3(cameraPosX, cameraPosY, cameraPosZ);
        cameraLookAt = -cameraPosition[0];
    }
}

void createTennisCourt(GLuint shaderProgram) {
    // Tennis court grass
    glBindTexture(GL_TEXTURE_2D, tennisCourtGrassTextureID);
    translationMatrix = translate(mat4(1.0f), groundPoint);
    scalingMatrix = scale(mat4(1.0f), vec3(50.0f, 0.05f, 108.34f));
    groundTennisHierarchicalMatrix = translationMatrix;
    worldMatrix = groundTennisHierarchicalMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis court lines
    // Left Line
    glBindTexture(GL_TEXTURE_2D, tennisCourtLineTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(-25.0f, 0.05f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 0.05f, 108.34f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Right Line
    translationMatrix = translate(mat4(1.0f), vec3(25.0f, 0.05f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 0.05f, 108.34f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Far Line
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.05f, -54.17f));
    scalingMatrix = scale(mat4(1.0f), vec3(50.0f, 0.05f, 0.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Near Line
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.05f, 54.17f));
    scalingMatrix = scale(mat4(1.0f), vec3(50.0f, 0.05f, 0.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Singles Left Line
    translationMatrix = translate(mat4(1.0f), vec3(-18.73f, 0.05f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 0.05f, 108.34f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Singles Right Line
    translationMatrix = translate(mat4(1.0f), vec3(18.73f, 0.05f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 0.05f, 108.34f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Center Line
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.05f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 0.05f, 58.34f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Inner Far Line
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.05f, -29.17f));
    scalingMatrix = scale(mat4(1.0f), vec3(37.46f, 0.05f, 0.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Inner Near Line
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.05f, 29.17f));
    scalingMatrix = scale(mat4(1.0f), vec3(37.46f, 0.05f, 0.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis Net Side 1
    glBindTexture(GL_TEXTURE_2D, silverPoleTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(-24.0f, 2.45f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 4.9f, 0.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis Net Side 2
    translationMatrix = translate(mat4(1.0f), vec3(24.0f, 2.45f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 4.9f, 0.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis Net
    SetUniform1Value(colorShaderProgram, "isTransparent", 1);
    glBindTexture(GL_TEXTURE_2D, tennisNetTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 2.07f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), tennisScaling);
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);
    SetUniform1Value(colorShaderProgram, "isTransparent", 0);

    // Tennis Net Top Part
    glBindTexture(GL_TEXTURE_2D, whiteTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 4.2f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(48.0f, 0.25f, 0.25f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Pole - 1
    glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[0]);
    translationMatrix = translate(mat4(1.0f), vec3(-28.0f, 1.0f, 8.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 2.0f, 0.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Pole - 2
    translationMatrix = translate(mat4(1.0f), vec3(-28.0f, 1.0f, -8.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Score Board
    glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[1]);
    translationMatrix = translate(mat4(1.0f), vec3(-28.0f, 6.0f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 8.0f, 20.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Inner background - Left
    glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[2]);
    translationMatrix = translate(mat4(1.0f), vec3(-27.7f, 6.0f, 5.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.05f, 6.8f, 8.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Inner background - Right
    translationMatrix = translate(mat4(1.0f), vec3(-27.7f, 6.0f, -5.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // P1
    glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[3]);
    translationMatrix = translate(mat4(1.0f), vec3(-27.62f, 8.0f, 5.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.01f, 2.8f, 2.8f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // P2
    glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[4]);
    translationMatrix = translate(mat4(1.0f), vec3(-27.62f, 8.0f, -5.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // P1 - score
    glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[5 + (p1Score < 5 ? p1Score : 0)]);
    translationMatrix = translate(mat4(1.0f), vec3(-27.62f, 5.0f, 5.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.01f, 2.5f, 2.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // P2 - score
    glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[5 + (p2Score < 5 ? p2Score : 0)]);
    translationMatrix = translate(mat4(1.0f), vec3(-27.62f, 5.0f, -5.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);
}

void drawWorld(GLuint shaderProgram, float xNeg, float xPos, float zNeg, float zPos, float yGround, float scaling) {
    // Draw textured geometry
    glActiveTexture(GL_TEXTURE0);
    mat4 rotatingLeftMatrix = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 rotatingRightMatrix = rotate(mat4(1.0f), radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));

    glBindTexture(GL_TEXTURE_2D, worldTextures[0]);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, -1.1f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(300.00f, 0.05f, 300.00f));
    groundTennisHierarchicalMatrix = translationMatrix;
    worldMatrix = groundTennisHierarchicalMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // back
    scalingMatrix = scale(mat4(1.0f), vec3(scaling / 15) + vec3(0.1f, 0.0f, 0.0f));
    for (float i = xNeg; i < xPos; i++) {
        glBindVertexArray(itemsVAO[0]);
        glBindTexture(GL_TEXTURE_2D, worldTextures[1]);
        translationMatrix = translate(mat4(1.0f), vec3(i + 4.0f, yGround + 3.6f, zNeg - 10.0f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[0], GL_UNSIGNED_INT, nullptr);
        i = i + 13;
    }
    // front
    for (float i = xNeg; i < xPos; i++) {
        translationMatrix = translate(mat4(1.0f), vec3(i + 5.0f, yGround + 3.6f, zPos + 10.0f));
        worldMatrix = translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[0], GL_UNSIGNED_INT, nullptr);
        i = i + 13;
    }

    //left
    scalingMatrix = scale(mat4(1.0f), vec3(scaling / 15) + vec3(0.0f, 0.0f, 0.2f));
    for (float i = zNeg; i < zPos; i++) {
        translationMatrix = translate(mat4(1.0f), vec3(xNeg - 7.0f, yGround + 3.6f, i + 5.0f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[0], GL_UNSIGNED_INT, nullptr);
        i = i + 13;
    }

    //Right
    for (float i = zNeg; i < zPos; i++) {
        translationMatrix = translate(mat4(1.0f), vec3(xPos + 7.0f, yGround + 3.6f, i + 5.0f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingLeftMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[0], GL_UNSIGNED_INT, nullptr);
        i = i + 13;
    }

    // Chair on left in court
    glBindVertexArray(itemsVAO[6]);
    glBindTexture(GL_TEXTURE_2D, worldTextures[2]);
    translationMatrix = translate(mat4(1.0f), vec3(xPos + 3.0f, yGround, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(scaling / 10.0f, scaling / 8.0f, scaling / 8.0f));
    worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawElements(GL_TRIANGLES_ADJACENCY_EXT, itemsVertices[1], GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(itemsVAO[2]);
    glBindTexture(GL_TEXTURE_2D, worldTextures[8]);
    translationMatrix = translate(mat4(1.0f), vec3(xPos + 0.8f, yGround + 5.0f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(scaling / 10.0f));
    worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix * rotatingRightMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawElements(GL_TRIANGLES, itemsVertices[2], GL_UNSIGNED_INT, nullptr);

    for (int count = 0; count < 2; count++) {
        float pushback = 10 * count;
        for (float i = zNeg; i < zPos; i++) {
            float x = xNeg - 10 - pushback;

            glBindVertexArray(itemsVAO[1]);
            glBindTexture(GL_TEXTURE_2D, worldTextures[3]);
            translationMatrix = translate(mat4(1.0f), vec3(x - 1.5f, yGround - 0.3f, i));
            scalingMatrix = scale(mat4(1.0f), vec3(scaling) + vec3(0.0f, 1.0f, 0.0f));
            worldMatrix = translationMatrix * scalingMatrix * rotatingLeftMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawElements(GL_TRIANGLES, itemsVertices[1], GL_UNSIGNED_INT, nullptr);

            glBindVertexArray(itemsVAO[2]);
            glBindTexture(GL_TEXTURE_2D, worldTextures[8]);
            translationMatrix = translate(mat4(1.0f), vec3(x, yGround + 2.5f, i - 1.8f));
            scalingMatrix = scale(mat4(1.0f), vec3(scaling / 10.0f));
            worldMatrix = translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawElements(GL_TRIANGLES, itemsVertices[2], GL_UNSIGNED_INT, nullptr);

            translationMatrix = translate(mat4(1.0f), vec3(x, yGround + 2.7f, i + 3.0f));
            worldMatrix = translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawElements(GL_TRIANGLES, itemsVertices[2], GL_UNSIGNED_INT, nullptr);

            i = i + 15 + count * 2.5;
        }
    }
    for (float i = zNeg; i < zPos; i++) {
        glBindVertexArray(itemsVAO[3]);
        translationMatrix = translate(mat4(1.0f), vec3(xPos + 12.0f, yGround - 0.3f, i));
        scalingMatrix = scale(mat4(1.0f), vec3(scaling / 30.0f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[3], GL_UNSIGNED_INT, nullptr);
        i = i + 10;
    }

    float randX[] = {30.0f, -70.0f, -74.0f, 70.0f, 55.0f, 12.0f, 60.0f, -95.0f};
    float randZ[] = {-70.0f, 30.0f, -70.0f, -30.0f, 70.0f, 80.0f, 12.0f, -10.0f};

    for (int i = 0; i < 7; i++) {
        glBindVertexArray(itemsVAO[4]);
        glBindTexture(GL_TEXTURE_2D, worldTextures[5]);
        translationMatrix = translate(mat4(1.0f), vec3(randX[i], yGround + 0.7f, randZ[i]));
        scalingMatrix = scale(mat4(1.0f), vec3(scaling * 1.5f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[4], GL_UNSIGNED_INT, nullptr);

        i++;

        glBindVertexArray(itemsVAO[5]);
        glBindTexture(GL_TEXTURE_2D, worldTextures[6]);
        translationMatrix = translate(mat4(1.0f), vec3(randX[i], yGround + 0.7f, randZ[i]));
        scalingMatrix = scale(mat4(1.0f), vec3(scaling * 2.5f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[5], GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);
    // End Frame
}

/**
 * Draw a frame
 */
void drawFrame(GLuint shaderProgram) {
    // Draw textured geometry
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(cubeVAO);
    // Surrounding cube
    glBindTexture(GL_TEXTURE_2D, blueBoxTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(200.0f, 200.0f, 200.0f));
    worldMatrix = translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Sphere 1 (Tennis ball)
    glBindVertexArray(sphereVAO);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    translationMatrix = translate(mat4(1.0f), ballPosition);
    scalingMatrix = scale(mat4(1.0f), ballScaling);
    worldMatrix = translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawElements(GL_TRIANGLE_STRIP, sphereVerticesCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(cubeVAO);

    // Create 2 players
    for (int i = 0; i < 2; i++) {
        // Cube 1 (Arm bottom)
        glBindTexture(GL_TEXTURE_2D, skin1TextureID);
        translationMatrix = translate(mat4(1.0f), modelPosition[i]);
        scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 3.3f, 0.42f));
        rotationMatrix =
                defaultArmRotationMatrix[0] * armRotationMatrix[i][0] * defaultArmTranslationMatrix[0];
        modelHierarchicalMatrix = yRotationMatrix[i] * translationMatrix *
                                  rotationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 2 (Arm top)
        glBindTexture(GL_TEXTURE_2D, skin2TextureID);
        translationMatrix = translate(mat4(1.0f), vec3(.0f, 1.5f, 0.0f));
        rotationMatrix =
                defaultArmRotationMatrix[1] * armRotationMatrix[i][1] * defaultArmTranslationMatrix[1];
        scalingMatrix = scale(mat4(1.0f), vec3(0.25f, 1.66f, 0.25f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 3 (hand - palm)
        glBindTexture(GL_TEXTURE_2D, skin3TextureID);
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 1.16f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.66f, 0.66f, 0.25f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.1 (hand - thumb)
        glBindTexture(GL_TEXTURE_2D, skin4TextureID);
        translationMatrix = translate(mat4(1.0f), vec3(-0.22f, 0.08f, 0.16f));
        rotationMatrix = rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.16f, 0.16f, 0.42f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.2 (hand - finger)
        translationMatrix = translate(mat4(1.0f), vec3(0.14f, 0.27f, 0.22f));
        rotationMatrix = rotate(mat4(1.0f), radians(-40.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.12f, 0.12f, 0.42f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.3 (hand - finger)
        translationMatrix = translate(mat4(1.0f), vec3(0.14f, 0.1f, 0.22f));
        rotationMatrix = rotate(mat4(1.0f), radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.14f, 0.14f, 0.58f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.4 (hand - finger)
        translationMatrix = translate(mat4(1.0f), vec3(0.16f, -0.03f, 0.22f));
        rotationMatrix = rotate(mat4(1.0f), radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.12f, 0.12f, 0.5f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.5 (hand - finger)
        translationMatrix = translate(mat4(1.0f), vec3(0.16f, -0.18f, 0.22f));
        rotationMatrix = rotate(mat4(1.0f), radians(-35.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.12f, 0.12f, 0.33f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 5 (Tennis handle)
        glBindTexture(GL_TEXTURE_2D, racketHandleTextureID);
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.16f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.16f, 2.33f, 0.16f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 6 (Tennis loop bottom)
        glBindTexture(GL_TEXTURE_2D, racketSideTextureID);
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 1.16f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(1.33f, 0.16f, 0.16f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 7 (Tennis loop left)
        translationMatrix = translate(mat4(1.0f), vec3(-0.66f, 0.83f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.16f, 1.66f, 0.16f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 8 (Tennis loop right)
        translationMatrix = translate(mat4(1.0f), vec3(0.66f, 0.83f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.16f, 1.66f, 0.16f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 9 (Tennis loop top)
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 1.66f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(1.33f, 0.16f, 0.16f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 10 (Racket net)
        glBindTexture(GL_TEXTURE_2D, racketNetTextureID);
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.83f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), racketNetScaling);
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);
        racketNetTransformation[i] = modelHierarchicalMatrix * translationMatrix;

        // Cube 9 (Letter P)
        glBindTexture(GL_TEXTURE_2D, playerTitleTextureID[i]);
        translationMatrix = translate(mat4(1.0f), vec3(-0.3f, 2.5f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.08f, 0.66f, 0.08f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        translationMatrix = translate(mat4(1.0f), vec3(0.16f, 0.33f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.42f, 0.08f, 0.08f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        translationMatrix = translate(mat4(1.0f), vec3(0.16f, 0.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.42f, 0.08f, 0.08f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        translationMatrix = translate(mat4(1.0f), vec3(0.33f, 0.16f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.08f, 0.33f, 0.08f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        if (i == 0) {
            // Cube 10 (Number 1)
            translationMatrix = translate(mat4(1.0f), vec3(0.66f, 0.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.08f, 0.75f, 0.08f));
            modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
            worldMatrix = modelHierarchicalMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        } else if (i == 1) {
            // Cube 12 (Number 2)
            translationMatrix = translate(mat4(1.0f), vec3(0.5f, 0.25f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.08f, 0.22f, 0.08f));
            modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
            worldMatrix = modelHierarchicalMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.16f, 0.08f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.42f, 0.08f, 0.08f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.16f, -0.25f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.08f, 0.75f, 0.08f));
            rotationMatrix = rotate(mat4(1.0f), radians(-30.0f), vec3(0.0f, 0.0f, 1.0f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.16f, -0.55f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.42f, 0.08f, 0.08f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);
        }
    }
    createTennisCourt(shaderProgram);
    drawWorld(colorShaderProgram, -25.0f, 25.0f, -54.0f, 54.0f, -0.5f, 2);

    glBindVertexArray(0);
    // End Frame
}

/**
 * Reset the ball position according to the player's turn
 */
void resetBall(int player) {
    bounce = 0, lastBounceRacket = 0;
    if (player == 1) {
        ballPosition = vec3(5.0f, 20.0f, 35.0f);
        ballScaling = vec3(0.25f, 0.25f, 0.25f);
        ballVelocity = vec3(0.0f, 0.0f, 0.0f);
    } else {
        ballPosition = vec3(-5.0f, 20.0f, -35.0f);
        ballScaling = vec3(0.25f, 0.25f, 0.25f);
        ballVelocity = vec3(0.0f, 0.0f, 0.0f);
    }
}

/**
 * Update the ball position
 */
void updateBallPosition() {
    // Racket Position
    racket1Position = racketNetTransformation[0][3];
    racket2Position = racketNetTransformation[1][3];
    vec3 racket1NormalWorld = vec3(transpose(inverse(racketNetTransformation[0])) * vec4(vec3(0.0f, 0.0f, 1.0f), 0.0f));
    vec3 racket2NormalWorld = vec3(
            transpose(inverse(racketNetTransformation[1])) * vec4(vec3(0.0f, 0.0f, -1.0f), 0.0f));
    vec3 tennisNetPoint = vec3(transpose(inverse(racketNetTransformation[0])) * vec4(vec3(0.0f, 0.0f, 0.0f), 1.0f));
    racket1Normal = normalize(racket1NormalWorld);
    racket2Normal = normalize(racket2NormalWorld);

    // Update ball position
    ballVelocity += vec3(0.0f, -GRAVITY, 0.0f) * dt;

    // Check ball bounce on the ground
    if (intersectsGround(groundPoint, groundNormal)) {
        ballVelocity.y = abs(ballVelocity.y * DAMPING);
        bounce++;
    }

    // Check ball bounce on the racket 1
    if (intersectsRacket(racket1Position, racket1Normal, racketNetScaling.x, racketNetScaling.y)) {
        // Reflect the ball's velocity off racket1
        ballVelocity = ballVelocity - 3.0f * dot(ballVelocity, racket1Normal) * racket1Normal;
        bounce = 0;
        lastBounceRacket = 1;
    }

    // Check ball bounce on the racket 2
    if (intersectsRacket(racket2Position, racket2Normal, racketNetScaling.x, racketNetScaling.y)) {
        // Reflect the ball's velocity off racket2
        ballVelocity = ballVelocity - 3.0f * dot(ballVelocity, racket2Normal) * racket2Normal;
        bounce = 0;
        lastBounceRacket = 2;
    }

    // Check ball bounce on the net
    if (intersectsNet(tennisNetPosition, tennisNetNormal, tennisScaling.x, tennisScaling.y)) {
        // Reflect the ball's velocity off the net
        ballVelocity = ballVelocity - 2.0f * dot(ballVelocity, tennisNetNormal) * tennisNetNormal;
    }

    ballPosition += dt * ballVelocity + 0.5f * dt * dt * vec3(0.0f, -GRAVITY, 0.0f);
    ballRotationAxis = mix(ballRotationAxis, ballAngularAxis, dt);
    ballRotationAngle += dt * ballAngularVelocity;
}

/**
 * Check for the score updates
 */
void checkScore() {
    if (bounce == 0) { // Check if ball is out of bounds
        if (ballPosition.x > 25.0f || ballPosition.x < -25.0f || ballPosition.z > 54.17f || ballPosition.z < -54.17f) {
            if (lastBounceRacket == 1) {
                p2Score = (p2Score + 1) % 5;
                nextServe = 2;
            } else if (lastBounceRacket == 2) {
                p1Score = (p1Score + 1) % 5;
                nextServe = 1;
            }
            resetBall(nextServe);
        }
    } else if (bounce == 1) { // Check if ball is on the same side of the last bounce & out of bounds on the other side
        if (lastBounceRacket == 1) {
            if (ballPosition.z > 0.0f) {
                p2Score = (p2Score + 1) % 5;
                nextServe = 2;
                resetBall(nextServe);
            } else if (ballPosition.z < 0.0f && (ballPosition.x > 25.0f || ballPosition.x < -25.0f)) {
                p1Score = (p1Score + 1) % 5;
                nextServe = 1;
                resetBall(nextServe);
            }
        } else if (lastBounceRacket == 2) {
            if (ballPosition.z < 0.0f) {
                p1Score = (p1Score + 1) % 5;
                nextServe = 1;
                resetBall(nextServe);
            } else if (ballPosition.z > 0.0f && (ballPosition.x > 25.0f || ballPosition.x < -25.0f)) {
                p2Score = (p2Score + 1) % 5;
                nextServe = 2;
                resetBall(nextServe);
            }
        }
    } else { // Check if ball has bounced multiple times on the other side of the net
        if (lastBounceRacket == 1) {
            if (ballPosition.z < 0) {
                p1Score = (p1Score + 1) % 5;
                nextServe = 1;
                resetBall(nextServe);
            }
        } else if (lastBounceRacket == 2) {
            if (ballPosition.z > 0) {
                p2Score = (p2Score + 1) % 5;
                nextServe = 2;
                resetBall(nextServe);
            }
        }
    }
}

/**
 * Process the updates after each frame
 */
void processUpdates() {
    // Get time difference since last frame
    dt = glfwGetTime() - lastFrameTime;
    lastFrameTime += dt;

    updateBallPosition();
    checkScore();

    // Update view matrix
    if (cameraIndex == 1) {
        cameraLookAt = -vec3(0.0f, 0.0f, 0.5f);
        cameraPosition[1] = vec3(modelPosition[0].x + 2.5f, modelPosition[0].y + 7.5f, modelPosition[0].z + 20.0f);
    } else if (cameraIndex == 2) {
        cameraLookAt = vec3(0.0f, 0.0f, 0.5f);
        cameraPosition[2] = vec3(modelPosition[1].x + 2.5f, modelPosition[1].y + 7.5f, modelPosition[1].z - 20.0f);
    } else {
        cameraLookAt = -cameraPosition[0];
    }
    viewMatrix = lookAt(cameraPosition[cameraIndex], cameraPosition[cameraIndex] + cameraLookAt, cameraUp);
    SetUniformMat4(colorShaderProgram, "viewMatrix", viewMatrix);

    // Update matrix with angle of rotation of the model
    yRotationMatrix[selectedModel] = rotate(mat4(1.0f), yRotationAngle[selectedModel], vec3(0.0f, 1.0f, 0.0f));

    // Update matrix with angle of rotation of the arm
    armXRotationMatrix[0][0] = rotate(mat4(1.0f), armXRotationAngle[0][0], vec3(1.0f, 0.0f, 0.0f));
    armXRotationMatrix[0][1] = rotate(mat4(1.0f), armXRotationAngle[0][1], vec3(1.0f, 0.0f, 0.0f));
    armXRotationMatrix[0][2] = rotate(mat4(1.0f), armXRotationAngle[0][2], vec3(1.0f, 0.0f, 0.0f));
    armYRotationMatrix[0][0] = rotate(mat4(1.0f), armYRotationAngle[0][0], vec3(0.0f, 1.0f, 0.0f));
    armYRotationMatrix[0][1] = rotate(mat4(1.0f), armYRotationAngle[0][1], vec3(0.0f, 1.0f, 0.0f));
    armYRotationMatrix[0][2] = rotate(mat4(1.0f), armYRotationAngle[0][2], vec3(0.0f, 1.0f, 0.0f));
    armZRotationMatrix[0][0] = rotate(mat4(1.0f), armZRotationAngle[0][0], vec3(0.0f, 0.0f, 1.0f));
    armZRotationMatrix[0][1] = rotate(mat4(1.0f), armZRotationAngle[0][1], vec3(0.0f, 0.0f, 1.0f));
    armZRotationMatrix[0][2] = rotate(mat4(1.0f), armZRotationAngle[0][2], vec3(0.0f, 0.0f, 1.0f));
    armRotationMatrix[0][0] = armXRotationMatrix[0][0] * armYRotationMatrix[0][0] * armZRotationMatrix[0][0];
    armRotationMatrix[0][1] = armXRotationMatrix[0][1] * armYRotationMatrix[0][1] * armZRotationMatrix[0][1];

    armXRotationMatrix[1][0] = rotate(mat4(1.0f), armXRotationAngle[1][0], vec3(1.0f, 0.0f, 0.0f));
    armXRotationMatrix[1][1] = rotate(mat4(1.0f), armXRotationAngle[1][1], vec3(1.0f, 0.0f, 0.0f));
    armXRotationMatrix[1][2] = rotate(mat4(1.0f), armXRotationAngle[1][2], vec3(1.0f, 0.0f, 0.0f));
    armYRotationMatrix[1][0] = rotate(mat4(1.0f), armYRotationAngle[1][0], vec3(0.0f, 1.0f, 0.0f));
    armYRotationMatrix[1][1] = rotate(mat4(1.0f), armYRotationAngle[1][1], vec3(0.0f, 1.0f, 0.0f));
    armYRotationMatrix[1][2] = rotate(mat4(1.0f), armYRotationAngle[1][2], vec3(0.0f, 1.0f, 0.0f));
    armZRotationMatrix[1][0] = rotate(mat4(1.0f), armZRotationAngle[1][0], vec3(0.0f, 0.0f, 1.0f));
    armZRotationMatrix[1][1] = rotate(mat4(1.0f), armZRotationAngle[1][1], vec3(0.0f, 0.0f, 1.0f));
    armZRotationMatrix[1][2] = rotate(mat4(1.0f), armZRotationAngle[1][2], vec3(0.0f, 0.0f, 1.0f));
    armRotationMatrix[1][0] = armXRotationMatrix[1][0] * armYRotationMatrix[1][0] * armZRotationMatrix[1][0];
    armRotationMatrix[1][1] = armXRotationMatrix[1][1] * armYRotationMatrix[1][1] * armZRotationMatrix[1][1];

    // Update Mouse Position
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
}

void reshape(GLFWwindow *handle, int width, int height) {
    glViewport(0, 0, width, height);
    projectionMatrix = perspective(70.0f,                                  // field of view in degrees
                                   (float) width / (float) height,       // aspect ratio
                                   0.01f, 1000.0f);                  // near and far (near > 0)
    SetUniformMat4(colorShaderProgram, "projectionMatrix", projectionMatrix);
}

/**
 * Initialize GLEW, GLFW and OpenGL version, create window, set callback functions and set other settings
 */
void initialize() {
    // Initialize GLFW and OpenGL version
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create Window and rendering context using GLFW, resolution is 1024x768
    window = glfwCreateWindow(1024, 768, "Comp371 - Project", nullptr, nullptr);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);

    if (window == nullptr) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to create GLEW" << endl;
        glfwTerminate();
        exit(-1);
    }

    // Enable Backface culling for performance
    //    glEnable(GL_CULL_FACE);

    // Enable Depth test for z-culling
    glEnable(GL_DEPTH_TEST);

    // Enable Line Width
    glEnable(GL_LINE_SMOOTH);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set mouse button and cursor position callback
    glfwSetMouseButtonCallback(window, mouseBtnCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetFramebufferSizeCallback(window, reshape);

    // Todo check if this is needed
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

    // Clear the screen to a light blue color
    glClearColor(0.63671875f, 0.8984375f, 0.99609375f, 1.0f);

    // Set draw point size
    glPointSize(5.0f);
}

/**
 * Initialize the frame buffer for the shadow map
 */
void initializeFrameBuffer() {
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
 * Load all the textures
 */
void loadAllTextures() {
    tennisCourtGrassTextureID = loadTexture("../assets/textures/tennis_court_grass.png");
    tennisCourtLineTextureID = loadTexture("../assets/textures/tennisCourtChalk.png");
    blueBoxTextureID = loadTexture("../assets/textures/blueBox.png");
    silverPoleTextureID = loadTexture("../assets/textures/silverPole.png");
    tennisNetTextureID = loadTexture("../assets/textures/tennis_net.png");
    playerTitleTextureID[0] = loadTexture("../assets/textures/red.png");
    playerTitleTextureID[1] = loadTexture("../assets/textures/blue.png");
    whiteTextureID = loadTexture("../assets/textures/white.png");
    skin1TextureID = loadTexture("../assets/textures/skin1.png");
    skin2TextureID = loadTexture("../assets/textures/skin2.png");
    skin3TextureID = loadTexture("../assets/textures/skin3.png");
    skin4TextureID = loadTexture("../assets/textures/skin4.png");
    racketHandleTextureID = loadTexture("../assets/textures/racketHandle.png");
    racketSideTextureID = loadTexture("../assets/textures/racket_side2.png");
    racketNetTextureID = loadTexture("../assets/textures/racket_net.png");
    ballTextureID = loadTexture("../assets/textures/ball.png");

    scoreBoardTextures[0] = loadTexture("../assets/textures/wood.png");
    scoreBoardTextures[1] = loadTexture("../assets/textures/darkGreen.png");
    scoreBoardTextures[2] = loadTexture("../assets/textures/yellow.png");
    scoreBoardTextures[3] = loadTexture("../assets/textures/p1.png");
    scoreBoardTextures[4] = loadTexture("../assets/textures/p2.png");
    scoreBoardTextures[5] = loadTexture("../assets/textures/score0.png");
    scoreBoardTextures[6] = loadTexture("../assets/textures/score15.png");
    scoreBoardTextures[7] = loadTexture("../assets/textures/score30.png");
    scoreBoardTextures[8] = loadTexture("../assets/textures/score45.png");
    scoreBoardTextures[9] = loadTexture("../assets/textures/score60.png");

    worldTextures[0] = loadTexture("../assets/textures/green.png");
    worldTextures[1] = loadTexture("../assets/textures/wood2.png");
    worldTextures[2] = loadTexture("../assets/textures/blueGlossy.png");
    worldTextures[3] = loadTexture("../assets/textures/greyGlossy.png");
    worldTextures[4] = loadTexture("../assets/textures/gray.png");
    worldTextures[5] = loadTexture("../assets/textures/tree.png");
    worldTextures[6] = loadTexture("../assets/textures/tree2.png");
    worldTextures[7] = loadTexture("../assets/textures/clayTexture.png");
    worldTextures[8] = loadTexture("../assets/textures/figureSkin.png");
}

/**
 * Main function that builds and runs the application
 * <ul><li>Initializes GLFW, GLEW and OpenGL version</li>
 * <li>Creates a window using GLFW</li>
 * <li>Creates a Vertex Array Object (VAO) and Vertex Buffer Object (VBO) for the given vertex array</li>
 * <li>Creates a shader program from the vertex and fragment shader sources</li>
 * <li>Initializes the view and projection matrices</li>
 * <li>Draws the frame</li>
 * <li>Processes the updates after each frame</li>
 * <li>Cleans up GLFW and OpenGL objects</li></ul
 */
int main(int argc, char *argv[]) {
    // Initialize GLEW, GLFW and OpenGL version, create window, set callback functions and other settings
    initialize();

    // Load all textures
    loadAllTextures();

    // Define and upload geometry to the GPU here ...
    string cubePath = "../assets/models/cube_model.obj";
    string spherePath = "../assets/models/sphere_model.obj";

    cubeVAO = setupModelVBO(cubePath, cubeVerticesCount);
    sphereVAO = setupModelEBO(spherePath, sphereVerticesCount);

    string itemPath[] = {
            "../assets/models/fence.obj",
            "../assets/models/metalChair.obj",
            "../assets/models/sittingMan.obj",
            "../assets/models/standingMan.obj",
            "../assets/models/tree.obj",
            "../assets/models/palmTree.obj",
            "../assets/models/refereeChair.obj"
    };
    for (int i = 0; i < 7; i++) {
        itemsVAO[i] = setupModelEBO(itemPath[i], itemsVertices[i]);
    }

    // Compile and link shaders, and set program to be used for rendering
    depthShaderProgram = loadSHADER("../assets/shaders/vertexDepthShader.glsl",
                                    "../assets/shaders/fragmentDepthShader.glsl");

    colorShaderProgram = loadSHADER("../assets/shaders/vertexShader.glsl",
                                    "../assets/shaders/fragmentShader.glsl");

    // Set projection matrix for shader
    projectionMatrix = perspective(70.0f, 1024.0f / 768.0f, 0.01f, 1000.0f);

    // Set initial view matrix
    viewMatrix = lookAt(cameraPosition[cameraIndex], cameraPosition[cameraIndex] + cameraLookAt, cameraUp);

    // Set View and Projection matrices on color shader
    SetUniformMat4(colorShaderProgram, "viewMatrix", viewMatrix);
    SetUniformMat4(colorShaderProgram, "projectionMatrix", projectionMatrix);

    // Initialize Frame Buffer
    initializeFrameBuffer();

    float lightAngleOuter = 30.0;
    float lightAngleInner = 20.0;

    // Set light parameters on color shader
    SetUniform1Value(colorShaderProgram, "lightCutoffInner", cos(radians(lightAngleInner)));
    SetUniform1Value(colorShaderProgram, "lightCutoffOuter", cos(radians(lightAngleOuter)));
    SetUniformVec3(colorShaderProgram, "lightColor", vec3(1.0, 1.0, 1.0));
    SetUniformVec3(colorShaderProgram, "objectColor", vec3(1.0, 1.0, 1.0));
    SetUniform1Value(colorShaderProgram, "isSpotLightOn", 0);
    SetUniform1Value(colorShaderProgram, "isShadowOn", 0);
    SetUniform1Value(colorShaderProgram, "isTransparent", 0);

    // Entering Main Loop
    while (!glfwWindowShouldClose(window)) {
        float lightNearPlane = 0.01f;
        float lightFarPlane = 100.0f;
        vec3 lightFocus(0.01);      // the point in 3D space the light "looks" at
        // light parameters
        if (cameraIndex == 3) {
            lightPosition = cameraPosition[cameraIndex];
            lightProjectionMatrix = perspective(radians(60.0f),
                                                (float) DEPTH_MAP_TEXTURE_SIZE / (float) DEPTH_MAP_TEXTURE_SIZE,
                                                lightNearPlane,
                                                lightFarPlane);
            SetUniform1Value(colorShaderProgram, "isShadowOn", 0);
        } else {
            lightPosition = vec3(0.0f, 0.0f, 30.0f);
            lightProjectionMatrix = perspective(radians(10.0f),
                                                (float) DEPTH_MAP_TEXTURE_SIZE / (float) DEPTH_MAP_TEXTURE_SIZE,
                                                lightNearPlane,
                                                lightFarPlane);
        }
        vec3 lightDirection = normalize(lightFocus - lightPosition);
        mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0.0f, 1.0f, 0.0f));
        mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

        // Set light space matrix on both shaders
        SetUniformMat4(depthShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);
        SetUniformMat4(colorShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);

        // Set light far and near planes on scene shader
        SetUniform1Value(colorShaderProgram, "lightNearPlane", lightNearPlane);
        SetUniform1Value(colorShaderProgram, "lightFarPlane", lightFarPlane);

        // Set light position on scene shader
        SetUniformVec3(colorShaderProgram, "lightPosition", lightPosition);

        // Set light direction on scene shader
        SetUniformVec3(colorShaderProgram, "lightDirection", lightDirection);

        // Set the view matrix for first person camera and send to both shaders
        viewMatrix = lookAt(cameraPosition[cameraIndex], cameraPosition[cameraIndex] + cameraLookAt, cameraUp);
        SetUniformMat4(colorShaderProgram, "viewMatrix", viewMatrix);

        // Set view position on scene shader
        SetUniformVec3(colorShaderProgram, "viewPosition", cameraPosition[cameraIndex]);

        // First pass
        glUseProgram(depthShaderProgram);
        glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        drawFrame(depthShaderProgram);
        SetUniformMat4(depthShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);

        // Second pass
        glUseProgram(colorShaderProgram);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
        glViewport(0, 0, frameBufferWidth, frameBufferHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set View and Projection matrices before drawing the scene
        SetUniformMat4(colorShaderProgram, "viewMatrix", viewMatrix);
        SetUniformMat4(colorShaderProgram, "projectionMatrix", projectionMatrix);
        SetUniformMat4(colorShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);

        glBindTexture(GL_TEXTURE_2D, depthMapFBO);

        // Draw scene
        drawFrame(colorShaderProgram);

        // Swap the back and front buffers
        glfwSwapBuffers(window);

        // Poll, process events and handle inputs
        handleInputs();

        // Process parameter updates
        processUpdates();
    }

    // Terminate GLFW
    glfwTerminate();

    return 0;
}