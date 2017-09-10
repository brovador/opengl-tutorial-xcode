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
#include <glm.hpp>
using namespace glm;

#include "vertexInfo.hpp"
#include "shader.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768


//IDs
GLuint vertexArrayID;
GLuint programID;
GLuint matrixID;

//Buffers
GLuint vertexBuffer;
GLuint colorBuffer;

//Matrices
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;



void init()
{
    programID = LoadShaders("/tmp/simple.vert", "/tmp/simple.frag");
    matrixID = glGetUniformLocation(programID, "MVP");
    
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    //Init matrices
    Projection = glm::perspective(
                                  glm::radians(45.f),
                                  (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
                                  0.1f,
                                  100.f
                                  );
    View = glm::lookAt(
                       glm::vec3(4, 3, -3),
                       glm::vec3(0, 0, 0),
                       glm::vec3(0, 1, 0)
                       );
}

void draw()
{
    glm::mat4 Model = glm::mat4(1.0f);
    
    //Model = glm::translate(Model, glm::vec3(-1, 0, 0));
    //Model = glm::rotate(Model, angle, glm::vec3(0, 0, 1));
    //Model = glm::scale(Model, glm::vec3(.5f, .5f, .5f));
    
    glm::mat4 mvp = Projection * View * Model;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glUseProgram(programID);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void end()
{
    glDeleteBuffers(1, &vertexBuffer);
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
    GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
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
    
    do{
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
