#ifndef SHADER_H
#define SHADER_H

#include "../../../libs/glad/glad.h"
#include <GLFW/glfw3.h>


unsigned int create_shader_program(const char* vertex_shader_path, const char* fragment_shader_path);

#endif
