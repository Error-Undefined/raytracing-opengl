#include "../../libs/glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"

#include "shaders/shader.h"

static void create_texture(s_graphics* g)
{
    glGenTextures(1, &g->texture_id);
    glBindTexture(GL_TEXTURE_2D, g->texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g->img_width, g->img_height, 0, GL_BGR, GL_UNSIGNED_BYTE, g->img_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

int create_graphics(s_graphics* g, int w, int h)
{

    // Init glfw
    if (!glfwInit())
    {
      return -1;
    }

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    g->mode = mode;
  
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
    GLFWwindow* window = glfwCreateWindow(w, h, "Test title", NULL, NULL);
    
    if(!window)
    {
      return -2;
    }
    g->window=window;

    glfwMakeContextCurrent(window);
  
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
      return -3;
    }
    glViewport(0, 0, w, h);
  
    // double dX = ((float)g->mode->width/(float)g->mode->height);
    double dX = 1.0;
    double dY = 1.0;
  
    //Set up all the vertex info
    //Two triangles that make up a square
    GLfloat vertices[] = {
        //pos                 // coords   
        dX * 1.0f, dY * 1.0f, 0.0f, 1.0f, 0.0f, //top, right
        dX * 1.0f, dY *-1.0f, 0.0f, 1.0f, 1.0f, //bottom, right
        dX *-1.0f, dY *-1.0f, 0.0f, 0.0f, 1.0f, //bottom, left
        dX *-1.0f, dY * 1.0f, 0.0f, 0.0f, 0.0f  //top, left
    };

    //Triangles
    GLuint indices[] = {
        0, 1, 3, //first 
        1, 2, 3  //second
    };

    create_texture(g);

    g->shader = create_shader_program("./src/graphics/shaders/vertex.vert", "./src/graphics/shaders/fragment.frag");
    glUseProgram(g->shader);

    GLuint vbo, ebo;
    glGenVertexArrays(1, &g->vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(g->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //glfwSetInputMode(g->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    //glfwSetCursorPos(g->window, 0, 0);

    return 0;
}


void *render_graphics(void* v_g)
{
  s_graphics* g = (s_graphics*) v_g;
  while(!glfwWindowShouldClose(g->window))
  {
    glBindTexture(GL_TEXTURE_2D, g->texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0 ,g->img_width, g->img_height, GL_RGB, GL_UNSIGNED_BYTE, g->img_data);

    glBindVertexArray(g->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);    

    glFlush();
  }
  stop_graphics(g);

  return NULL;
}

void stop_graphics(s_graphics* g)
{
  glfwTerminate();
}


