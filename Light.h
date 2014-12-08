#ifndef LIGHT_H
#define LIGHT_H

#include "Vect.h"
#include "Color.h"

class Light {
	Vect position;
	Color color;
	
public:
	Light(); //defaults to x direction
	Light(Vect, Color);
	
	//method functions:
	virtual double getDistanceFrom(Vect intersect) { return 0; }
	virtual Vect getDirectionFrom(Vect intersect) { return Vect(0,0,0); }
	virtual Color getColor() { return Color (1,1,1,0); }
	
};

Light::Light () {
	position = Vect(0, 0, 0);
	color = Color(1, 1, 1, 0);
}

Light::Light (Vect pos, Color col) {
	position = pos;
	color = col;
}

#endif