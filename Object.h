#ifndef OBJECT_H
#define OBJECT_H

#include "Ray.h"
#include "Vect.h"
#include "Color.h"

//base scene object class (defaults in here are for null objects)
class Object {
	
public:
	Object(); //defaults to x direction
	
	//method functions:
	virtual Color getColor() { return Color(0, 0, 0, 0); }
	
	virtual Vect getNormalAt(Vect) {
		return Vect(0,0,0);
	}
	
	virtual double findIntersection(Ray ray) {
		return 0;
	}
	
};

Object::Object () {}

#endif