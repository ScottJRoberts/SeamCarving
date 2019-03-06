# SeamCarving
Attached is a seam carving codebase written to be later computed in parallel. It utilizes greyscale PGM images. It utilizes the CImg libary. Currently WIP. 

To compile 
g++-8 -fopenmp -Wall -pedantic -o imgTest seamCarvingSequential.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11

To run
./imgTest <filenameOfPhoto.pgm> <number of seams to remove>
