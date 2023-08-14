#include <iostream>
#include <fstream>
#include <sstream>
#include "GL/glew.h"

using namespace std;

/**
 * Reads the shader file and returns the contents as a string.
 */
string readShaderFile(const char* filePath) {
    ifstream shaderFile(filePath);
    if (!shaderFile) {
        cerr << "Error: Could not open shader file: " << filePath << endl;
        return "";
    }

    stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

/**
 * Compiles a shader for the given shader source code.
 */
unsigned int compileShader(const char* shaderSource, unsigned int shaderType) {
    unsigned int shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderSource, nullptr);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        cerr << "Shader compilation error:\n" << infoLog << endl;
        glDeleteShader(shaderID);
        return 0;
    }

    return shaderID;
}

/**
 * Creates a shader program from the given vertex and fragment shader source code.
 */
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    if (!vertexShader || !fragmentShader)
        return 0;

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        cerr << "Shader program linking error:\n" << infoLog << endl;
        glDeleteProgram(program);
        return 0;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

/**
 * Reads the shader source code from the given file path and creates a shader program.
 */
int loadSHADER(const char* vertexPath, const char* fragmentPath) {
    string vertexShaderSource = readShaderFile(vertexPath);
    string fragmentShaderSource = readShaderFile(fragmentPath);

    int shaderProgram = createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
    return shaderProgram;
}