C++ Raytracer
===========
This is a raytracer project of a C++ programming course. It supports rendering simple geometry (cubes, spheres, triangles) on the CPU with a basic point light model.

Here is an animation rendered with this raytracer:
[![Rendering demonstraion](https://github.com/user-attachments/assets/6d21406a-ab48-4f73-a616-321d04ad2a36)](https://www.youtube.com/watch?v=lnThyhbEaXc)

#### Example scene
Build the "example" cmake projekt. Before executing it, add a file in your build directory named "resource-paths.txt" and write down the relative path to the "sdf" folder in it.

#### The video
Build the "moviemaker" cmake project. Copy the "movie" folder into the build directory and add 2 empty folders to it named "files" and "images".  
The video will be rendered as single .ppm images. You can combine them to a video e.g. with ffmpeg:  
` ffmpeg -r 24 -i frame%4d.ppm -vf format=yuv420p video.mp4`
