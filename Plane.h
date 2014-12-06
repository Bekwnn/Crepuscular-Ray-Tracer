#ifndef PLANE_H
#define PLANE_H

#include "Vect.h"
#include "Color.h"
#include "Object.h"

class Plane : public Object{
	Vect normal;
	double distance;
	Color color;
	
public:
	Plane(); //defaults to x direction
	Plane(Vect, double, Color);
	
	//method functions:
	Vect getNormal() { return normal; }
	double getDistance() { return distance; }
	Color getColor() { return color; }
	
	virtual Vect getNormalAt(Vect point) {
		return normal;
	}
	
	virtual double findIntersection(Ray ray) {
		Vect ray_direction = ray.getDirection();
		
		double ndotr = ray_direction.dot(normal);
		
		if (ndotr == 0) {
			//ray is parallel to plane
			return -1;
		}
		else {
			double ndoton = normal.dot(ray.getOrigin() + -(normal * distance));
			return -1*ndoton/ndotr;
		}
	}
};

Plane::Plane () {
	normal = Vect(1, 0, 0);
	distance = 0;
	color = Color(0.5, 0.5, 0.5, 0);
}

Plane::Plane (Vect norm, double dst, Color col) {
	normal = norm;
	distance = dst;
	color = col;
}

#endif