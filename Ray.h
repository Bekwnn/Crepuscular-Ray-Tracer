#ifndef RAY_H
#define RAY_H

#include "Vect.h"

class Ray {
	Vect origin, direction;
	
public:
	Ray(); //defaults to x direction
	Ray(Vect, Vect);
	
	//method functions:
	Vect getOrigin() { return origin; }
	Vect getDirection() { return direction; }
	
};

Ray::Ray () {
	origin = Vect(0, 0, 0);
	direction = Vect(1, 0, 0);
}

Ray::Ray (Vect o, Vect d) {
	origin = o;
	direction = d;
}

#endif