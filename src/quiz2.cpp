/**
 * COMP371: Computer Graphics |
 * Quiz 2 |
 * Name: Bhavya Ruparelia |
 * Student ID: 40164863
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
GLuint groundTextureID, blueBoxTextureID, skin1TextureID, skin2TextureID, skin3TextureID, skin4TextureID,
        racketHandleTextureID, racketSideTextureID, racketNetTextureID, ballTextureID, redAlphabetTextureID,
        greenAlphabetTextureID, blueAlphabetTextureID, yellowAlphabetTextureID, whiteTextureID, silverPoleTextureID,
        tennisNetTextureID;

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
vec3 cameraPosition[] = {vec3(0.0f, 7.5f, 40.0f),
                         vec3(-5.0f, 2.25f, -20.0f),
                         vec3(-5.0f, 2.25f, 20.0f),
                         vec3(0.0f, 7.5f, 60.0f)};
vec3 cameraLookAt(0.0f, 0.0f, -0.5f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);
int cameraIndex = 0;

float lastBtnClick = 0.0f;

// Time frame parameter
float lastFrameTime = (float) glfwGetTime();

// Model parameters for model transform
vec3 modelPosition[] = {vec3(-5.0f, 2.25f, -20.0f),
                        vec3(-5.0f, 2.25f, 20.0f)};
float modelScalingFactor = 1.0f;
vec3 modelScalingVector(modelScalingFactor, modelScalingFactor, modelScalingFactor);
mat4 modelScalingMatrix = scale(mat4(1.0f), modelScalingVector);
float yRotationAngle[] = {0.0f, 0.0f}, xWorldRotationAngle = 0.0f, yWorldRotationAngle = 0.0f;
float rotateByAngle = 5.0f, otherRotateByAngle = 0.5f;
mat4 yRotationMatrix[] = {rotate(mat4(1.0f), yRotationAngle[0], vec3(0.0f, 1.0f, 0.0f)),
                          rotate(mat4(1.0f), yRotationAngle[1], vec3(0.0f, 1.0f, 0.0f))};
mat4 xWorldRotationMatrix = rotate(mat4(1.0f), xWorldRotationAngle, vec3(1.0f, 0.0f, 0.0f));
mat4 yWorldRotationMatrix = rotate(mat4(1.0f), yWorldRotationAngle, vec3(0.0f, 1.0f, 0.0f));
mat4 worldRotationMatrix = xWorldRotationMatrix * yWorldRotationMatrix;
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
        translate(mat4(1.0f), vec3(0.75f, 5.0f, 0.0f)),
        translate(mat4(1.0f), vec3(0.375f, 2.5f, 0.375f))
};
int selectedFigure = 0;
int selectedModel = 0;

// Light parameters
vec3 lightPosition;
mat4 lightProjectionMatrix;
bool isSpotLightOn = false;
bool isShadowOn = false;

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(1);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(1);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
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
    cameraLookAt = vec3(0.0f, 0.0f, -0.5f);
    cameraUp = vec3(0.0f, 1.0f, 0.0f);
    cameraIndex = 0;
    modelPosition[0] = vec3(-5.0f, 2.25f, -20.0f), modelPosition[1] = vec3(-5.0f, 2.25f, 20.0f);
    yRotationAngle[0] /= -2, yRotationAngle[1] /= -2;
    xWorldRotationAngle /= -2, yWorldRotationAngle /= -2;
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
            cameraPosition[cameraIndex] -= vec3(0.0f, 0.0f, 0.5f);
        } else {
            cameraPosition[cameraIndex] += vec3(0.0f, 0.0f, 0.5f);
        }
    }

    // Move camera horizontally
    if (isRightMBClicked) {
        if (deltaX > 0) {
            cameraPosition[cameraIndex] -= vec3(0.5f, 0.0f, 0.0f);
        } else {
            cameraPosition[cameraIndex] += vec3(0.5f, 0.0f, 0.0f);
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
        cameraPosition[cameraIndex] += cameraSideVector * dt;
    }
}

/**
 * Handle inputs from keyboard
 */
void handleInputs() {
    // Poll for and process events
    glfwPollEvents();

    // Move model to a random position
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        modelPosition[selectedModel].x = (rand() % 90) - 45;
        modelPosition[selectedModel].z = (rand() % 90) - 45;
    }

    // Scale up model
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        modelScalingFactor = modelScalingFactor < 10.0f ? modelScalingFactor + 0.01f : 10.0f;
    }

    // Scale down model
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        modelScalingFactor = modelScalingFactor > 0.01f ? modelScalingFactor - 0.01f : 0.01f;
    }

    // Exit program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Move model to +z direction
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        modelPosition[selectedModel].z =
                modelPosition[selectedModel].z > -44.0f ? modelPosition[selectedModel].z - 0.5f : -44.0f;
    }

    // Move model to -z direction
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        modelPosition[selectedModel].z =
                modelPosition[selectedModel].z < 44.0f ? modelPosition[selectedModel].z + 0.5f : 44.0f;
    }

    // Rotate model to the left around y-axis by 5 degrees
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK)) {
            // 5 degrees = 0.0872665 radians
            yRotationAngle[selectedModel] -= radians(rotateByAngle);
        } else {
            modelPosition[selectedModel].x =
                    modelPosition[selectedModel].x > -20.5f ? modelPosition[selectedModel].x - 0.5f : -20.5f;
        }
    }

    // Rotate model to the right around y-axis by 5 degrees
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK)) {
            yRotationAngle[selectedModel] += radians(rotateByAngle);
        } else {
            modelPosition[selectedModel].x =
                    modelPosition[selectedModel].x < 20.5f ? modelPosition[selectedModel].x + 0.5f : 20.5f;
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

    // Rotate model to the left around x-axis by 1 degree
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        yWorldRotationAngle += radians(otherRotateByAngle);
    }

    // Rotate model to the right around x-axis by 1 degree
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        yWorldRotationAngle -= radians(otherRotateByAngle);
    }

    // Rotate model to the left around y-axis by 1 degree
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        xWorldRotationAngle += radians(otherRotateByAngle);
    }

    // Rotate model to the right around y-axis by 1 degree
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        xWorldRotationAngle -= radians(otherRotateByAngle);
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
}

/**
 * Draw a frame
 */
void drawFrame(GLuint shaderProgram) {
    // Draw textured geometry
    glActiveTexture(GL_TEXTURE0);

    // Square - Ground
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, groundTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, -1.0f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(36.0f, 0.02f, 78.0f));
    groundTennisHierarchicalMatrix = worldRotationMatrix * translationMatrix;
    worldMatrix = groundTennisHierarchicalMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Surrounding cube
    glBindTexture(GL_TEXTURE_2D, blueBoxTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(150.0f, 150.0f, 150.0f));
    worldMatrix = worldRotationMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis Net Side 1
    glBindTexture(GL_TEXTURE_2D, silverPoleTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(-17.0f, 6.0f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(1.0f, 12.0f, 1.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis Net Side 2
    translationMatrix = translate(mat4(1.0f), vec3(17.0f, 6.0f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(1.0f, 12.0f, 1.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis Net Side 3
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 6.0f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(1.0f, 12.0f, 1.0f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis Net
    glBindTexture(GL_TEXTURE_2D, tennisNetTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 5.5f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(34.0f, 10.0f, 0.5f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Tennis Net Top Part
    glBindTexture(GL_TEXTURE_2D, whiteTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 10.5f, 0.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(34.0f, 1.0f, 0.25f));
    worldMatrix = groundTennisHierarchicalMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

    // Sphere 1 (Tennis ball)
    glBindVertexArray(sphereVAO);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    translationMatrix = translate(mat4(1.0f), vec3(0.0f, 10.0f, 10.0f));
    scalingMatrix = scale(mat4(1.0f), vec3(0.3f, 0.3f, 0.3f));
    worldMatrix = worldRotationMatrix * translationMatrix * scalingMatrix;
    SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
    glDrawElements(GL_TRIANGLES, sphereVerticesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(cubeVAO);

    // Create 2 players
    for (int i = 0; i < 2; i++) {
        // Cube 1 (Arm bottom)
        glBindTexture(GL_TEXTURE_2D, skin1TextureID);
        translationMatrix = translate(mat4(1.0f), modelPosition[i]);
        scalingMatrix = scale(mat4(1.0f), vec3(1.5f, 10.0f, 1.25f));
        rotationMatrix = defaultArmRotationMatrix[0] * armRotationMatrix[0] * defaultArmTranslationMatrix[0];
        modelHierarchicalMatrix = worldRotationMatrix * yRotationMatrix[i] * modelScalingMatrix * translationMatrix *
                                  rotationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 2 (Arm top)
        glBindTexture(GL_TEXTURE_2D, skin2TextureID);
        translationMatrix = translate(mat4(1.0f), vec3(-0.25f, 4.5f, -0.375f));
        rotationMatrix = defaultArmRotationMatrix[1] * armRotationMatrix[1] * defaultArmTranslationMatrix[1];
        scalingMatrix = scale(mat4(1.0f), vec3(0.75f, 5.0f, 0.75f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 3 (hand - palm)
        glBindTexture(GL_TEXTURE_2D, skin3TextureID);
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 3.5f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(2.0f, 2.0f, 0.75f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.1 (hand - thumb)
        glBindTexture(GL_TEXTURE_2D, skin4TextureID);
        translationMatrix = translate(mat4(1.0f), vec3(-0.65f, 0.25f, 0.5f));
        rotationMatrix = rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 0.5f, 1.25f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.2 (hand - finger)
        translationMatrix = translate(mat4(1.0f), vec3(0.4f, 0.8f, 0.65f));
        rotationMatrix = rotate(mat4(1.0f), radians(-40.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.35f, 0.35f, 1.5f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.3 (hand - finger)
        translationMatrix = translate(mat4(1.0f), vec3(0.4f, 0.3f, 0.65f));
        rotationMatrix = rotate(mat4(1.0f), radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.4f, 0.4f, 1.75f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.4 (hand - finger)
        translationMatrix = translate(mat4(1.0f), vec3(0.5f, -0.1f, 0.65f));
        rotationMatrix = rotate(mat4(1.0f), radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.35f, 0.35f, 1.5f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 4.5 (hand - finger)
        translationMatrix = translate(mat4(1.0f), vec3(0.5f, -0.55f, 0.65f));
        rotationMatrix = rotate(mat4(1.0f), radians(-35.0f), vec3(0.0f, 1.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.35f, 0.35f, 1.0f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * rotationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 5 (Tennis handle)
        glBindTexture(GL_TEXTURE_2D, racketHandleTextureID);
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 3.0f, 0.5f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 7.0f, 0.5f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 6 (Tennis loop bottom)
        glBindTexture(GL_TEXTURE_2D, racketSideTextureID);
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 3.5f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(4.0f, 0.5f, 0.5f));
        modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
        worldMatrix = modelHierarchicalMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 7 (Tennis loop left)
        translationMatrix = translate(mat4(1.0f), vec3(-2.0f, 2.5f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 5.0f, 0.5f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 8 (Tennis loop right)
        translationMatrix = translate(mat4(1.0f), vec3(2.0f, 2.5f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(0.5f, 5.0f, 0.5f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 9 (Tennis loop top)
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 5.0f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(4.0f, 0.5f, 0.5f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        // Cube 10 (Racket net)
        glBindTexture(GL_TEXTURE_2D, racketNetTextureID);
        translationMatrix = translate(mat4(1.0f), vec3(0.0f, 2.5f, 0.0f));
        scalingMatrix = scale(mat4(1.0f), vec3(4.0f, 5.0f, 0.5f));
        worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
        SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

        if (i == 0) {
            // Cube 9 (Letter E)
            glBindTexture(GL_TEXTURE_2D, redAlphabetTextureID);
            translationMatrix = translate(mat4(1.0f), vec3(-1.5f, 7.5f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.25f, 2.0f, 0.25f));
            modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
            worldMatrix = modelHierarchicalMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.5f, 1.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(1.25f, 0.25f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(1.25f, 0.25f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.5f, -1.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(1.25f, 0.25f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            // Cube 10 (Letter L)
            glBindTexture(GL_TEXTURE_2D, greenAlphabetTextureID);
            translationMatrix = translate(mat4(1.0f), vec3(2.0f, 0.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.25f, 2.25f, 0.25f));
            modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
            worldMatrix = modelHierarchicalMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.5f, -1.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(1.25f, 0.25f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);
        } else if (i == 1) {
            // Cube 11 (Letter I)
            glBindTexture(GL_TEXTURE_2D, blueAlphabetTextureID);
            translationMatrix = translate(mat4(1.0f), vec3(-1.0f, 7.5f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.25f, 2.0f, 0.25f));
            modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
            worldMatrix = modelHierarchicalMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(1.25f, 0.25f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.0f, -1.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(1.25f, 0.25f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            // Cube 12 (Letter A)
            glBindTexture(GL_TEXTURE_2D, yellowAlphabetTextureID);
            translationMatrix = translate(mat4(1.0f), vec3(1.5f, 0.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.25f, 2.0f, 0.25f));
            modelHierarchicalMatrix = modelHierarchicalMatrix * translationMatrix;
            worldMatrix = modelHierarchicalMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(1.0f, 0.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(0.25f, 2.0f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.5f, 0.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(1.25f, 0.25f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);

            translationMatrix = translate(mat4(1.0f), vec3(0.5f, 1.0f, 0.0f));
            scalingMatrix = scale(mat4(1.0f), vec3(1.25f, 0.25f, 0.25f));
            worldMatrix = modelHierarchicalMatrix * translationMatrix * scalingMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, cubeVerticesCount);
        }
    }

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
    cameraPosition[1] = vec3(modelPosition[0].x, modelPosition[0].y + 2.5, modelPosition[0].z - 0.5);
    cameraPosition[2] = vec3(modelPosition[1].x, modelPosition[1].y + 2.5, modelPosition[1].z + 0.5);
    viewMatrix = lookAt(cameraPosition[cameraIndex], cameraPosition[cameraIndex] + cameraLookAt, cameraUp);
    SetUniformMat4(colorShaderProgram, "viewMatrix", viewMatrix);

    // Update matrix with scaling factor of the model
    modelScalingVector = vec3(modelScalingFactor, modelScalingFactor, modelScalingFactor);
    modelScalingMatrix = scale(mat4(1.0f), modelScalingVector);

    // Update matrix with angle of rotation of the model
    yRotationMatrix[selectedModel] = rotate(mat4(1.0f), yRotationAngle[selectedModel], vec3(0.0f, 1.0f, 0.0f));

    // Update matrices with angle of rotation of the world. Reset the rotation angle if home key is pressed
    xWorldRotationMatrix = rotate(mat4(1.0f), xWorldRotationAngle, vec3(1.0f, 0.0f, 0.0f));
    yWorldRotationMatrix = rotate(mat4(1.0f), yWorldRotationAngle, vec3(0.0f, 1.0f, 0.0f));
    worldRotationMatrix = xWorldRotationMatrix * yWorldRotationMatrix;

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
    window = glfwCreateWindow(1024, 768, "Comp371 - Quiz 2", nullptr, nullptr);
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
                 DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

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
    groundTextureID = loadTexture("../assets/textures/ground.png");
    blueBoxTextureID = loadTexture("../assets/textures/blue_box.png");
    silverPoleTextureID = loadTexture("../assets/textures/silver_pole.png");
    tennisNetTextureID = loadTexture("../assets/textures/tennis_net.png");
    redAlphabetTextureID = loadTexture("../assets/textures/red.png");
    greenAlphabetTextureID = loadTexture("../assets/textures/green.png");
    blueAlphabetTextureID = loadTexture("../assets/textures/blue.png");
    yellowAlphabetTextureID = loadTexture("../assets/textures/yellow.png");
    whiteTextureID = loadTexture("../assets/textures/white.png");
    skin1TextureID = loadTexture("../assets/textures/skin1.png");
    skin2TextureID = loadTexture("../assets/textures/skin2.png");
    skin3TextureID = loadTexture("../assets/textures/skin3.png");
    skin4TextureID = loadTexture("../assets/textures/skin4.png");
    racketHandleTextureID = loadTexture("../assets/textures/racket_handle.png");
    racketSideTextureID = loadTexture("../assets/textures/racket_side2.png");
    racketNetTextureID = loadTexture("../assets/textures/racket_net.png");
    ballTextureID = loadTexture("../assets/textures/ball.png");
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
