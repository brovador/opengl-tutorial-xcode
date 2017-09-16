//
//  main.cpp
//  OpenGLTutorial
//
//  Created by Jesús Bartús Sancho on 9/9/17.
//  Copyright © 2017 Jesús Bartús Sancho. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <glfw3.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>

#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <gtx/matrix_decompose.hpp>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
using namespace glm;

#include "vertexInfo.hpp"
#include "shader.hpp"
#include "objloader.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768


typedef struct{
    int vertexCount;
    glm::vec3 color;
    
    GLuint vertexBufferID;
    GLuint normalBufferID;
    
    glm::mat4 position;
    glm::mat4 rotation;
    glm::mat4 scale;
} ObjectInfo;

//VAO
GLuint vertexArrayID;

//Shader IDs
GLuint programID;
GLuint uniformMVPID;
GLuint uniformMID;
GLuint uniformVID;
GLuint uniformPID;
GLuint uniformColorID;
GLuint uniformLightPosID;
GLuint uniformViewPosID;

//Matrices
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;

//Objects
#define OBJECTS 2
ObjectInfo objCube;
ObjectInfo objLight;
ObjectInfo* objects[OBJECTS] = {
    &objCube, &objLight
};

GLFWwindow* window;
glm::vec3 viewPos = glm::vec3(4, 3, -10);


void init()
{
    programID = LoadShaders("/tmp/simple.vert", "/tmp/simple.frag");
    uniformMVPID = glGetUniformLocation(programID, "MVP");
    uniformMID = glGetUniformLocation(programID, "M");
    uniformVID = glGetUniformLocation(programID, "V");
    uniformPID = glGetUniformLocation(programID, "P");
    uniformColorID = glGetUniformLocation(programID, "Color");
    uniformLightPosID = glGetUniformLocation(programID, "LightPos");
    uniformViewPosID = glGetUniformLocation(programID, "ViewPos");
    
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec2 > uvs;
    std::vector< glm::vec3 > normals; // Won't be used at the moment.
    bool res = loadOBJ("/tmp/suzanne.obj", vertices, uvs, normals);
    
    if (!res) {
        fprintf(stderr, "No obj file loaded\n");
        return;
    }
    
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    
    for (int i = 0; i < OBJECTS; i++) {
        ObjectInfo *objectInfo = objects[i];
        
        objectInfo->vertexCount = (int)vertices.size();
        
        glGenBuffers(1, &(objectInfo->vertexBufferID));
        glBindBuffer(GL_ARRAY_BUFFER, objectInfo->vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(glm::vec3),
                     &vertices[0],
                     GL_STATIC_DRAW);
        
        glGenBuffers(1, &(objectInfo->normalBufferID));
        glBindBuffer(GL_ARRAY_BUFFER, objectInfo->normalBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     normals.size() * sizeof(glm::vec3),
                     &normals[0],
                     GL_STATIC_DRAW);
    }
    
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}


void setupScene()
{
    //Init matrices
    Projection = glm::perspective(
                                  glm::radians(45.f),
                                  (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
                                  0.1f,
                                  100.f
                                  );
    View = glm::lookAt(
                       viewPos,
                       glm::vec3(0, 0, 0),
                       glm::vec3(0, 1, 0)
                       );
    
    //Init objects
    objLight.color = glm::vec3(1,1,1);
    objLight.position = glm::translate(glm::mat4(1), glm::vec3(4, 3, 3));
    
    objCube.color = glm::vec3(1, 0, 0);
}

float speed = 0.01f;

void update()
{
    objCube.rotation *= glm::rotate(speed, glm::vec3(0, 1, 0));
    
    if (glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS){
        objLight.position = glm::translate(objLight.position, glm::vec3(0, 1, 0) * speed * 10.f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        objLight.position = glm::translate(objLight.position, glm::vec3(0, -1, 0) * speed * 10.f);
    }
    if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS){
        objLight.position = glm::translate(objLight.position, glm::vec3(1, 0, 0) * speed * 10.f);
    }
    if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS){
        objLight.position = glm::translate(objLight.position, glm::vec3(-1, 0, 0) * speed * 10.f);
    }
    if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS){
        objLight.position = glm::translate(objLight.position, glm::vec3(0, 0, 1) * speed * 10.f);
    }
    if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS){
        objLight.position = glm::translate(objLight.position, glm::vec3(0, 0, -1) * speed * 10.f);
    }

}


vec3 GetPositionVector(glm::mat4 positionMatrix)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(positionMatrix, scale, rotation, translation, skew, perspective);
    return translation;
}


void draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glUseProgram(programID);
    
    for (int i = 0; i < OBJECTS; i++) {
        ObjectInfo *objectInfo = objects[i];
        
        glm::mat4 Model = objectInfo->position * objectInfo->rotation * objectInfo->scale;
        
        glm::mat4 MVP = Projection * View * Model;
        glUniformMatrix4fv(uniformMVPID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(uniformMID, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(uniformVID, 1, GL_FALSE, &View[0][0]);
        glUniformMatrix4fv(uniformPID, 1, GL_FALSE, &Projection[0][0]);
        glUniform3fv(uniformColorID, 1, glm::value_ptr(objectInfo->color));
        //TODO: use decompose to recover original position from transformation
        glUniform3fv(uniformLightPosID, 1, glm::value_ptr(GetPositionVector(objLight.position)));
        glUniform3fv(uniformViewPosID, 1, glm::value_ptr(viewPos));
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, objectInfo->vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, objectInfo->normalBufferID);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glDrawArrays(GL_TRIANGLES, 0, objectInfo->vertexCount);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }
}

void end()
{
    for (int i = 0; i < OBJECTS; i++) {
        ObjectInfo *objectInfo = objects[i];
        glDeleteBuffers(1, &(objectInfo->vertexBufferID));
        glDeleteBuffers(1, &(objectInfo->normalBufferID));
    }
    glDeleteVertexArrays(1, &vertexArrayID);
}

int main(int argc, const char * argv[]) {
    
    if (!glfwInit()) {
        fprintf(stderr, "Error initializing GLFW\n");
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Tutorials", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    init();
    setupScene();
    
    do{
        update();
        draw();
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );

    end();
    
    glfwTerminate();
    
    return 0;
}
