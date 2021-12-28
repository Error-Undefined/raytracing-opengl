#include <stdio.h>

#include "fileout.h"
#include "vector3.h"

void* write_file(void* args)
{
  struct write_file_args* args_cast = (struct write_file_args*) args;

  const char* path = args_cast->path;
  int_color* color_data = args_cast->color_data;
  int height = args_cast->height;
  int width = args_cast->width;

  FILE* fp;
  fp = fopen(path, "w+");

  if (fp == NULL)
  {
    args_cast->result = -1;
    return NULL;
  }
  //Write .ppm file header
  fputs("P3\n", fp);
  fprintf(fp, "%d %d\n255\n", width, height);

  for (int h = 0; h < height; h++)
  {
    for(int w = 0;  w < width; w++)
    {
      int r = (int) (color_data[h*width + w].r);
      int g = (int) (color_data[h*width + w].g);
      int b = (int) (color_data[h*width + w].b);
      fprintf(fp, "%d %d %d\n", r, g, b);
    }
  }

  fclose(fp);
  args_cast->result = 0;
  return NULL;
}
