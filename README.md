## Intro

Starting a small project of a raytracer written in C.

Following raytracing in a weekend: https://raytracing.github.io/books/RayTracingInOneWeekend.html

## Building and running
You will need the following to run:
- GLFW
- OpenGL (4.1 or higher)

To build, simply do 

    make

Consider tinkering with the makefile, it does not do fancy compiler detection or similar so you have to set this yourself. 

For running, the executable is called `main`. You can specify the resolution of the render (in pixels) as

    ./main <h> <w>

If you do not specify a resolution, it will default to 800x450.

The render will be created as a .ppm file, `render.ppm`

## Features
- Raytracing renderer with choosable pixel samples and ray depth
- Spheres with three types of materials: lambertian, metal, and dielectric.
- Customizable camera: location, viewing direction, rotation and focal length
- Camera that also emulates a thin lens and expresses depth of field through an aperture and focus distance.
- Any resolution or aspect ratio for the output image

