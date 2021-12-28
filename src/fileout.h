#ifndef FILEOUT_H
#define FILEOUT_H

#include "vector3.h"

struct write_file_args
{
    const char* path; 
    int_color* color_data; 
    int height;
    int width;

    int result;
};

void* write_file(void* args);

#endif //FILEOUT_H
