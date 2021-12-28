#include <stdlib.h>
#include <stdio.h>

#include "../../../libs/glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.h"

static int shader_compile(const char* shader_path, unsigned int* shader, GLenum SHADER_TYPE)
{
    size_t file_size = 0;
    char* shader_code = NULL;

    *shader = glCreateShader(SHADER_TYPE);

    FILE* fp = fopen(shader_path, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "Could not open shader file\n");
        fclose(fp);
        exit(-1);
    }

    if( fseek(fp, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Could not get shader file length\n");  
        fclose(fp);
        exit(-1);
    }

    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    shader_code = malloc(file_size + 1*sizeof(char));
    if (shader_code == NULL)
    {
        fprintf(stderr, "Could not alloc RAM for shader\n");
        fclose(fp);
        exit(-1);
    }
    shader_code[file_size] = 0;

    if(fread(shader_code, sizeof(char), file_size, fp) != file_size)
    {
        fprintf(stderr, "Could not read shader file\n");
        fclose(fp);
        exit(-1);
    }

    glShaderSource(*shader, 1, (const char**) &shader_code, NULL);
    glCompileShader(*shader);

    int shader_compiled_successfully = 0;
    char shader_compilation_info[1024];

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &shader_compiled_successfully);
    if(!shader_compiled_successfully)
    {
        glGetShaderInfoLog(*shader, 1024, NULL, shader_compilation_info);
        printf("Shader compilation failed with error: %s\n", shader_compilation_info);
        fclose(fp);
        exit(-1);
    }
    return 0;
}

unsigned int create_shader_program(const char* vertex_shader_path, const char* fragment_shader_path)
{
    unsigned int vertex_shader, fragment_shader;

    shader_compile(vertex_shader_path, &vertex_shader, GL_VERTEX_SHADER);
    shader_compile(fragment_shader_path, &fragment_shader, GL_FRAGMENT_SHADER);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    if(vertex_shader == 0 || fragment_shader == 0)
    {
        fprintf(stderr, "Error in loading shader files\n");
    }

    glLinkProgram(shader_program);
    int link_success = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &link_success);

    if(!link_success)
    {
        char link_info[1024];
        glGetProgramInfoLog(shader_program, 1024, NULL, link_info);
        fprintf(stderr, "Error during linking of shader program: %s\n", link_info);
        exit(-1);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
