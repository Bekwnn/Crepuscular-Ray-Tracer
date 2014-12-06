#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"
#include "Vect.h"
#include "Color.h"

class PointLight : public Light {
	Vect position;
	Color color;
	
public:
	PointLight(); //defaults to x direction
	PointLight(Vect, Color);
	
	//method functions:
	Vect getPosition() { return position; }
	Color getColor() { return color; }
	
};

PointLight::PointLight () {
	position = Vect(0, 0, 0);
	color = Color(1, 1, 1, 0);
}

PointLight::PointLight (Vect pos, Color col) {
	position = pos;
	color = col;
}

#endif