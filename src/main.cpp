/**
 * COMP371: Computer Graphics |
 * Final Project |
 * Name: Bhavya Ruparelia | Devansh Vaidya | Jananee Aruboribaran |
 * Student ID: 40164863 | 40165987 |  |
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

// Textures
GLuint tennisCourtGrassTextureID, tennisCourtLineTextureID, blueBoxTextureID, skin1TextureID, skin2TextureID,
        skin3TextureID, skin4TextureID, racketHandleTextureID, racketSideTextureID, racketNetTextureID, ballTextureID,
        whiteTextureID, silverPoleTextureID, tennisNetTextureID;
GLuint playerTitleTextureID[2];

int p1Score = 0, p2Score = 0;

// Frame buffer object for shadow mapping
GLuint depthMapFbo;

// Vertex array objects for textured cube and sphere
GLuint cubeVAO, sphereVAO;

int cubeVerticesCount, sphereVerticesCount;

// Matrix declarations
mat4 worldMatrix, viewMatrix, projectionMatrix, rotationMatrix, translationMatrix, scalingMatrix, modelHierarchicalMatrix,
        groundTennisHierarchicalMatrix;

// Tilting parameters for camera
float dt = 0.0f;
float cameraVerticalAngle = 0.0f;

// Mouse parameters
bool isLeftMBClicked = false, isRightMBClicked = false, isMiddleMBClicked = false;
double lastMousePosX = 0.0f, lastMousePosY = 0.0f;

// Camera parameters for view transform
vec3 cameraSideVector = vec3(1.0f, 1.0f, 1.0f);
vec3 cameraPosition[] = {vec3(0.0f, 20.0f, 70.0f),
                         vec3(-5.0f, 2.25f, -20.0f),
                         vec3(-5.0f, 2.25f, 20.0f),
                         vec3(0.0f, 7.5f, 60.0f)};
vec3 cameraLookAt = cameraPosition[0];
vec3 cameraUp(0.0f, 1.0f, 0.0f);
int cameraIndex = 0;
float cameraRotationSpeed = 50.0f;
float cameraRotationAngle = 0.0f;
float cameraRadius = length(cameraPosition[0]);
float cameraTheta = atan(cameraPosition[0].z, cameraPosition[0].x);
float cameraPhi = asin(cameraPosition[0].y / cameraRadius);
float cameraPosX, cameraPosY, cameraPosZ;

float lastBtnClick = 0.0f;

// Time frame parameter
float lastFrameTime = (float) glfwGetTime();

// Model parameters for model transform
vec3 modelPosition[] = {vec3(-5.0f, 0.25f, -40.0f),
                        vec3(-5.0f, 0.25f, 40.0f)};
float yRotationAngle[] = {0.0f, 0.0f};
float rotateByAngle = 5.0f, otherRotateByAngle = 0.5f;
mat4 yRotationMatrix[] = {rotate(mat4(1.0f), yRotationAngle[0], vec3(0.0f, 1.0f, 0.0f)),
                          rotate(mat4(1.0f), yRotationAngle[1], vec3(0.0f, 1.0f, 0.0f))};
float armXRotationAngle[] = {0.0f, 0.0f, 0.0f};
float armYRotationAngle[] = {0.0f, 0.0f, 0.0f};
float armZRotationAngle[] = {0.0f, 0.0f, 0.0f};
mat4 armXRotationMatrix[] = {rotate(mat4(1.0f), armXRotationAngle[0], vec3(1.0f, 0.0f, 0.0f)),
                             rotate(mat4(1.0f), armXRotationAngle[1], vec3(1.0f, 0.0f, 0.0f)),
                             rotate(mat4(1.0f), armXRotationAngle[2], vec3(1.0f, 0.0f, 0.0f))};
mat4 armYRotationMatrix[] = {rotate(mat4(1.0f), armYRotationAngle[0], vec3(0.0f, 1.0f, 0.0f)),
                             rotate(mat4(1.0f), armYRotationAngle[1], vec3(0.0f, 1.0f, 0.0f)),
                             rotate(mat4(1.0f), armYRotationAngle[2], vec3(0.0f, 1.0f, 0.0f))};
mat4 armZRotationMatrix[] = {rotate(mat4(1.0f), armZRotationAngle[0], vec3(0.0f, 0.0f, 1.0f)),
                             rotate(mat4(1.0f), armZRotationAngle[1], vec3(0.0f, 0.0f, 1.0f)),
                             rotate(mat4(1.0f), armZRotationAngle[2], vec3(0.0f, 0.0f, 1.0f))};
mat4 armRotationMatrix[] = {armXRotationMatrix[0] * armYRotationMatrix[0] * armZRotationMatrix[0],
                            armXRotationMatrix[1] * armYRotationMatrix[1] * armZRotationMatrix[1],
                            armXRotationMatrix[2] * armYRotationMatrix[2] * armZRotationMatrix[2]};
mat4 defaultArmRotationMatrix[] = {
        rotate(mat4(1.0f), radians(-60.0f), vec3(0.0f, 0.0f, 1.0f)),
        rotate(mat4(1.0f), radians(60.0f), vec3(0.0f, 0.0f, 1.0f))
};
mat4 defaultArmTranslationMatrix[] = {
        translate(mat4(1.0f), vec3(0.5f, 3.3f, 0.42f)),
        translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.0f))
};
int selectedFigure = 0;
int selectedModel = 0;

// Ball parameters
vec3 ballPosition(0.0f, 10.0f, 10.0f);
vec3 ballVelocity(0.0f, 0.0f, 0.0f);

// Light parameters
vec3 lightPosition;
mat4 lightProjectionMatrix;
bool isSpotLightOn = false;
bool isShadowOn = false;

//VAO for world
int itemsVertices[12];
GLuint itemsVAO[12];
GLuint worldTextures[10], scoreBoardTextures[10];

void drawWorld(GLuint shaderProgram, float xPos, float xNeg, float zPos, float zNeg, float yGround, float scaling);

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
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> UVs;

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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(0);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(1);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
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
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;

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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(0);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *) nullptr);
    glEnableVertexAttribArray(1);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
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
    cameraPosition[0] = vec3(0.0f, 7.5f, 40.0f);
    cameraPosition[1] = vec3(-5.0f, 2.25f, -20.0f);
    cameraPosition[2] = vec3(-5.0f, 2.25f, 20.0f);
    cameraLookAt = -cameraPosition[0];
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    cameraIndex = 0;
    modelPosition[0] = vec3(-5.0f, 0.25f, -40.0f), modelPosition[1] = vec3(-5.0f, 0.25f, 40.0f);
    yRotationAngle[0] /= -2, yRotationAngle[1] /= -2;
    armXRotationAngle[0] /= -2, armYRotationAngle[0] /= -2, armZRotationAngle[0] /= -2;
    armXRotationAngle[1] /= -2, armYRotationAngle[1] /= -2, armZRotationAngle[1] /= -2;
    armXRotationAngle[2] /= -2, armYRotationAngle[2] /= -2, armZRotationAngle[2] /= -2;
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
        modelPosition[selectedModel].z =
                modelPosition[selectedModel].z > -54.17f ? modelPosition[selectedModel].z - 0.5f : -54.17f;
    }

    // Move model to -z direction
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        modelPosition[selectedModel].z =
                modelPosition[selectedModel].z < 54.17f ? modelPosition[selectedModel].z + 0.5f : 54.17f;
    }

    // Rotate model to the left around y-axis by 5 degrees
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK)) {
            // 5 degrees = 0.0872665 radians
            yRotationAngle[selectedModel] -= radians(rotateByAngle);
        } else {
            modelPosition[selectedModel].x =
                    modelPosition[selectedModel].x > -25.0f ? modelPosition[selectedModel].x - 0.5f : -25.0f;
        }
    }

    // Rotate model to the right around y-axis by 5 degrees
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK)) {
            yRotationAngle[selectedModel] += radians(rotateByAngle);
        } else {
            modelPosition[selectedModel].x =
                    modelPosition[selectedModel].x < 25.0f ? modelPosition[selectedModel].x + 0.5f : 25.0f;
        }
    }

    // Select 1st model
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        selectedModel = 0;
        cameraPosition[cameraIndex] = modelPosition[selectedModel] + vec3(0.0f, 7.75f, 50.0f);
    }

    // Select 2nd model
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        selectedModel = 1;
        cameraPosition[cameraIndex] = modelPosition[selectedModel] + vec3(0.0f, 7.75f, 50.0f);
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
        armXRotationAngle[selectedFigure] -= radians(otherRotateByAngle);
    }

    // Rotate arm in x-axis
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        armXRotationAngle[selectedFigure] += radians(otherRotateByAngle);
    }

    // Rotate arm in y-axis
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        armYRotationAngle[selectedFigure] -= radians(otherRotateByAngle);
    }

    // Rotate arm in y-axis
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        armYRotationAngle[selectedFigure] += radians(otherRotateByAngle);
    }

    // Rotate arm in z-axis
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        armZRotationAngle[selectedFigure] -= radians(otherRotateByAngle);
    }

    // Rotate arm in z-axis
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        armZRotationAngle[selectedFigure] += radians(otherRotateByAngle);
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
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, -1.0f, 0.0f));
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
    scalingMatrix = scale(mat4(1.0f), vec3(48.0f, 4.14f, 0.5f));
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

    // P1 - Score texture determination
    switch (p1Score) {
        case 0:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[5]);
            break;
        case 15:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[6]);
            break;
        case 30:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[7]);
            break;
        case 45:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[8]);
            break;
        case 60:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[9]);
            break;
    }

    // P1 - draw score
    translationMatrix = translate(mat4(1.0f), vec3(-27.62f, 5.0f, 5.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.01f, 2.5f, 2.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // P2 - Score texture determination
    switch (p2Score) {
        case 0:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[5]);
            break;
        case 15:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[6]);
            break;
        case 30:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[7]);
            break;
        case 45:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[8]);
            break;
        case 60:
            glBindTexture(GL_TEXTURE_2D, scoreBoardTextures[9]);
            break;
    }

    // P2 - score
    translationMatrix = translate(mat4(1.0f), vec3(-27.62f, 5.0f, -5.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);
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
    scalingMatrix = scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));
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
        rotationMatrix = defaultArmRotationMatrix[0] * armRotationMatrix[0] * defaultArmTranslationMatrix[0];
        modelHierarchicalMatrix = yRotationMatrix[i] * translationMatrix *
                                  rotationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 2 (Arm top)
        glBindTexture(GL_TEXTURE_2D, skin2TextureID);
        translationMatrix = translate(mat4(1.0f), vec3(.0f, 1.5f, 0.0f));
        rotationMatrix = defaultArmRotationMatrix[1] * armRotationMatrix[1] * defaultArmTranslationMatrix[1];
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
        scalingMatrix = scale(mat4(1.0f), vec3(1.33f, 1.66f, 0.16f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

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
 * Process the updates after each frame
 */
void processUpdates() {
    // Get time difference since last frame
    dt = glfwGetTime() - lastFrameTime;
    lastFrameTime += dt;

    // Update view matrix
    if (cameraIndex != 0) {
        cameraLookAt = -vec3(0.0f, 0.0f, 0.5f);
        cameraPosition[1] = vec3(modelPosition[0].x, modelPosition[0].y + 2.5, modelPosition[0].z - 0.5);
        cameraPosition[2] = vec3(modelPosition[1].x, modelPosition[1].y + 2.5, modelPosition[1].z + 0.5);
    } else {
        cameraLookAt = -cameraPosition[0];
    }
    viewMatrix = lookAt(cameraPosition[cameraIndex], cameraPosition[cameraIndex] + cameraLookAt, cameraUp);
    SetUniformMat4(colorShaderProgram, "viewMatrix", viewMatrix);

    // Update matrix with angle of rotation of the model
    yRotationMatrix[selectedModel] = rotate(mat4(1.0f), yRotationAngle[selectedModel], vec3(0.0f, 1.0f, 0.0f));

    // Update matrix with angle of rotation of the arm
    armXRotationMatrix[0] = rotate(mat4(1.0f), armXRotationAngle[0], vec3(1.0f, 0.0f, 0.0f));
    armXRotationMatrix[1] = rotate(mat4(1.0f), armXRotationAngle[1], vec3(1.0f, 0.0f, 0.0f));
    armXRotationMatrix[2] = rotate(mat4(1.0f), armXRotationAngle[2], vec3(1.0f, 0.0f, 0.0f));
    armYRotationMatrix[0] = rotate(mat4(1.0f), armYRotationAngle[0], vec3(0.0f, 1.0f, 0.0f));
    armYRotationMatrix[1] = rotate(mat4(1.0f), armYRotationAngle[1], vec3(0.0f, 1.0f, 0.0f));
    armYRotationMatrix[2] = rotate(mat4(1.0f), armYRotationAngle[2], vec3(0.0f, 1.0f, 0.0f));
    armZRotationMatrix[0] = rotate(mat4(1.0f), armZRotationAngle[0], vec3(0.0f, 0.0f, 1.0f));
    armZRotationMatrix[1] = rotate(mat4(1.0f), armZRotationAngle[1], vec3(0.0f, 0.0f, 1.0f));
    armZRotationMatrix[2] = rotate(mat4(1.0f), armZRotationAngle[2], vec3(0.0f, 0.0f, 1.0f));
    armRotationMatrix[0] = armXRotationMatrix[0] * armYRotationMatrix[0] * armZRotationMatrix[0];
    armRotationMatrix[1] = armXRotationMatrix[1] * armYRotationMatrix[1] * armZRotationMatrix[1];

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
    glGenFramebuffers(1, &depthMapFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);

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
    tennisCourtLineTextureID = loadTexture("../assets/textures/tennis_court_chalk.png");
    blueBoxTextureID = loadTexture("../assets/textures/blue_box.png");
    silverPoleTextureID = loadTexture("../assets/textures/silver_pole.png");
    tennisNetTextureID = loadTexture("../assets/textures/tennis_net.png");
    playerTitleTextureID[0] = loadTexture("../assets/textures/red.png");
    playerTitleTextureID[1] = loadTexture("../assets/textures/blue.png");
    whiteTextureID = loadTexture("../assets/textures/white.png");
    skin1TextureID = loadTexture("../assets/textures/skin1.png");
    skin2TextureID = loadTexture("../assets/textures/skin2.png");
    skin3TextureID = loadTexture("../assets/textures/skin3.png");
    skin4TextureID = loadTexture("../assets/textures/skin4.png");
    racketHandleTextureID = loadTexture("../assets/textures/racket_handle.png");
    racketSideTextureID = loadTexture("../assets/textures/racket_side2.png");
    racketNetTextureID = loadTexture("../assets/textures/racket_net.png");
    ballTextureID = loadTexture("../assets/textures/ball.png");

    scoreBoardTextures[0] = loadTexture("../assets/textures/wood.png");
    scoreBoardTextures[1] = loadTexture("../assets/textures/dark_green.png");
    scoreBoardTextures[2] = loadTexture("../assets/textures/yellow.png");
    scoreBoardTextures[3] = loadTexture("../assets/textures/p1.png");
    scoreBoardTextures[4] = loadTexture("../assets/textures/p2.png");
    scoreBoardTextures[5] = loadTexture("../assets/textures/score_0.png");
    scoreBoardTextures[6] = loadTexture("../assets/textures/score_15.png");
    scoreBoardTextures[7] = loadTexture("../assets/textures/score_30.png");
    scoreBoardTextures[8] = loadTexture("../assets/textures/score_45.png");
    scoreBoardTextures[9] = loadTexture("../assets/textures/score_60.png");

    worldTextures[0] = loadTexture("../assets/textures/green.png");
    worldTextures[1] = loadTexture("../assets/textures/wood2.png");
    worldTextures[2] = loadTexture("../assets/textures/blueGlossy.png");
    worldTextures[3] = loadTexture("../assets/textures/greyGlossy.png");
    worldTextures[4] = loadTexture("../assets/textures/gray.png");
    worldTextures[5] = loadTexture("../assets/textures/tree.png");
    worldTextures[6] = loadTexture("../assets/textures/tree2.png");
    worldTextures[7] = loadTexture("../assets/textures/clayTexture.png");
    worldTextures[8] = loadTexture("../assets/textures/stone.png");
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
            "../assets/models/MetalChair.obj",
            "../assets/models/MetalChair.obj",
            "../assets/models/MetalChair.obj",
            "../assets/models/tree.obj",
            "../assets/models/palmTree.obj",
            "../assets/models/CavePlatform1_Obj.obj",
            "../assets/models/rock_stone.obj",
            "../assets/models/Rocks.obj",
            "../assets/models/rabbit_orictolagus_01.obj",
            "../assets/models/sittingMan.obj",
            "../assets/models/standingMan.obj"
    };
    for (int i = 0; i < 12; i++) {
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
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
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

        glBindTexture(GL_TEXTURE_2D, depthMapFbo);

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


void drawWorld(GLuint shaderProgram, float xNeg, float xPos, float zNeg, float zPos, float yGround, float scaling) {
    // Draw textured geometry
    glActiveTexture(GL_TEXTURE0);
    mat4 rotatingLeftMatrix = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 rotatingRightMatrix = rotate(mat4(1.0f), radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));

    glBindTexture(GL_TEXTURE_2D, worldTextures[0]);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, -2.0f, 0.0f));
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
        translationMatrix = translate(mat4(1.0f), vec3(i + 4.0f, yGround + 3.0f, zNeg - 10.0f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[0], GL_UNSIGNED_INT, nullptr);
        i = i + 13;
    }
    // front
    for (float i = xNeg; i < xPos; i++) {
        translationMatrix = translate(mat4(1.0f), vec3(i + 5.0f, yGround + 3.0f, zPos + 10.0f));
        worldMatrix = translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[0], GL_UNSIGNED_INT, nullptr);
        i = i + 13;
    }

    //left
    scalingMatrix = scale(mat4(1.0f), vec3(scaling / 15) + vec3(0.0f, 0.0f, 0.2f));
    for (float i = zNeg; i < zPos; i++) {
        translationMatrix = translate(mat4(1.0f), vec3(xNeg - 7.0f, yGround + 3.0f, i + 5.0f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[0], GL_UNSIGNED_INT, nullptr);
        i = i + 13;
    }

    //Right
    for (float i = zNeg; i < zPos; i++) {
        translationMatrix = translate(mat4(1.0f), vec3(xPos + 7.0f, yGround + 3.0f, i + 5.0f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingLeftMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[0], GL_UNSIGNED_INT, nullptr);
        i = i + 13;
    }

    //chair on left in court
    glBindVertexArray(itemsVAO[1]);
    glBindTexture(GL_TEXTURE_2D, worldTextures[2]);
    translationMatrix = translate(mat4(1.0f), vec3(xPos + 2.0f, yGround, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(scaling) + vec3(0.0f, 1.0f, 0.0f));
    worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawElements(GL_TRIANGLES, itemsVertices[1], GL_UNSIGNED_INT, nullptr);

    for (int count = 0; count < 2; count++) {
        float pushback = 10 * count;
        for (float i = zNeg; i < zPos; i++) {
            float x = xNeg - 10 - pushback;

            glBindVertexArray(itemsVAO[1]);
            glBindTexture(GL_TEXTURE_2D, worldTextures[3]);
            translationMatrix = translate(mat4(1.0f), vec3(x - 1.5f, yGround - 1.0f, i));
            scalingMatrix = scale(mat4(1.0f), vec3(scaling) + vec3(0.0f, 1.0f, 0.0f));
            worldMatrix = translationMatrix * scalingMatrix * rotatingLeftMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawElements(GL_TRIANGLES, itemsVertices[1], GL_UNSIGNED_INT, nullptr);

            glBindVertexArray(itemsVAO[2]);
            glBindTexture(GL_TEXTURE_2D, worldTextures[4]);
            translationMatrix = translate(mat4(1.0f), vec3(x - 2.0, yGround - 1.0f, i - 2.2));
            scalingMatrix = scale(mat4(1.0f), vec3(scaling) + vec3(0.0f, 1.5f, 0.0f));
            worldMatrix = translationMatrix * scalingMatrix * rotatingLeftMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawElements(GL_TRIANGLES, itemsVertices[2], GL_UNSIGNED_INT, 0);

            glBindVertexArray(itemsVAO[10]);
            glBindTexture(GL_TEXTURE_2D, worldTextures[2]);
            translationMatrix = translate(mat4(1.0f), vec3(x, yGround + 3.0f, i - 2.2));
            scalingMatrix = scale(mat4(1.0f), vec3(scaling / 10.0f));
            worldMatrix = translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawElements(GL_TRIANGLES, itemsVertices[2], GL_UNSIGNED_INT, 0);

            glBindVertexArray(itemsVAO[2]);
            glBindTexture(GL_TEXTURE_2D, worldTextures[4]);
            translationMatrix = translate(mat4(1.0f), vec3(x - 2.0, yGround - 1.0f, i + 2.3));
            scalingMatrix = scale(mat4(1.0f), vec3(scaling) + vec3(0.0f, 1.5f, 0.0f));
            worldMatrix = translationMatrix * scalingMatrix * rotatingLeftMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawElements(GL_TRIANGLES, itemsVertices[2], GL_UNSIGNED_INT, 0);

            glBindVertexArray(itemsVAO[10]);
            glBindTexture(GL_TEXTURE_2D, worldTextures[2]);
            translationMatrix = translate(mat4(1.0f), vec3(x, yGround + 3.0f, i + 2.3));
            scalingMatrix = scale(mat4(1.0f), vec3(scaling / 10.0f));
            worldMatrix = translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawElements(GL_TRIANGLES, itemsVertices[2], GL_UNSIGNED_INT, 0);

            i = i + 15 + count * 2.5;
        }
    }
    for (float i = zNeg; i < zPos; i++) {
        glBindVertexArray(itemsVAO[3]);
        glBindTexture(GL_TEXTURE_2D, worldTextures[4]);
        translationMatrix = translate(mat4(1.0f), vec3(xPos + 12.0f, yGround - 1.0f, i));
        scalingMatrix = scale(mat4(1.0f), vec3(scaling) + vec3(0.0f, 1.0f, 0.0f));
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
        translationMatrix = translate(mat4(1.0f), vec3(randX[i], yGround, randZ[i]));
        scalingMatrix = scale(mat4(1.0f), vec3(scaling * 1.5f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[4], GL_UNSIGNED_INT, nullptr);

        i++;

        glBindVertexArray(itemsVAO[5]);
        glBindTexture(GL_TEXTURE_2D, worldTextures[6]);
        translationMatrix = translate(mat4(1.0f), vec3(randX[i], yGround, randZ[i]));
        scalingMatrix = scale(mat4(1.0f), vec3(scaling * 2.5f));
        worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawElements(GL_TRIANGLES, itemsVertices[5], GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(itemsVAO[6]);
    glBindTexture(GL_TEXTURE_2D, worldTextures[7]);
    translationMatrix = translate(mat4(1.0f), vec3(80.0f, yGround - 2, -80.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(scaling * 6));
    worldMatrix = translationMatrix * scalingMatrix * rotatingRightMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawElements(GL_TRIANGLES, itemsVertices[6], GL_UNSIGNED_INT, nullptr);

    glBindTexture(GL_TEXTURE_2D, worldTextures[8]);
    translationMatrix = translate(mat4(1.0f), vec3(65.0f, yGround - 2, -70.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(scaling * 3));
    worldMatrix = translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawElements(GL_TRIANGLES, itemsVertices[6], GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    // End Frame
}
