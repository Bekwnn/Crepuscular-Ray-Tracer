#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Vect.h"
#include "Color.h"
#include "Light.h"

class DirectionalLight : public Light{
	Vect direction;
	Color color;
	
public:
	DirectionalLight(); //defaults to x direction
	DirectionalLight(Vect, Color);
	
	//method functions:
	Vect getDirectionFrom(Vect ignorethis) { return (-direction); }
	Color getColor() { return color; }
};

DirectionalLight::DirectionalLight () {
	direction = Vect(1, 0, 0);
	color = Color(0.5, 0.5, 0.5, 0);
}

DirectionalLight::DirectionalLight (Vect dir, Color col) {
	direction = dir;
	color = col;
}

#endif