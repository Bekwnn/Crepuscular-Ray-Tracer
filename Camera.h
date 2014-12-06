#ifndef CAMERA_H
#define CAMERA_H

#include "Vect.h"

class Camera {
	Vect campos, camdir, camright, camdown;
	
public:
	Camera(); //defaults to x direction
	Camera(Vect, Vect, Vect, Vect);
	
	//method functions:
	Vect getCameraPosition() { return campos; }
	Vect getCameraDirection() { return camdir; }
	Vect getCameraRight() { return camright; }
	Vect getCameraDown() { return camdown; }
	
};

Camera::Camera () {
	campos = Vect(0, 0, 0);
	camdir = Vect(0, 0, 1);
	camright = Vect(1,0,0);
	camdown = Vect(0,-1,0);
}

Camera::Camera (Vect pos, Vect dir, Vect right, Vect down) {
	campos = pos;
	camdir = dir;
	camright = right;
	camdown = down;
}

#endif