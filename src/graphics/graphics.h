#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../../libs/glad/glad.h"
#include <GLFW/glfw3.h>

#include "../vector3.h"

struct graphics
{
    GLFWwindow* window;
    const GLFWvidmode* mode;
    GLuint texture_id;
    GLuint vao;
    GLuint shader;

    int_color* img_data;
    int img_width, img_height;
};

typedef struct graphics s_graphics;

int create_graphics(s_graphics* g, int w, int h);

void *render_graphics(void* g);

void stop_graphics(s_graphics* g);

#endif
