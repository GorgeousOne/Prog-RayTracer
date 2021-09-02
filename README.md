Programmiersprachen: Raytracer
===========

### Setup

#### Example scene

Build the "example" cmake projekt. Before executing it add a file in your build directory named "resource-paths.txt" and write down the relative path to the "sdf" folder in it.

#### The video

Build the "moviemaker" cmake project. Copy the "movie" folder into the build directory and add 2 empty folders to it named "files" and "images".  
The video will be rendered as single .ppm images. You can combine them to a video e.g. with ffmpeg:  
` ffmpeg -r 24 -i frame%4d.ppm -vf format=yuv420p video.mp4`
