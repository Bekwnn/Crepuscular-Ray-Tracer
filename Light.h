#ifndef LIGHT_H
#define LIGHT_H

#include "Vect.h"
#include "Color.h"

class Light {
	Vect position;
	Color color;
	
public:
	Light();
	
	//method functions:
	virtual Vect getPosition() { return Vect(0, 0, 0); }
	virtual Color getColor() { return Color(1, 1, 1, 0); }
	
};

Light::Light () {}

#endif