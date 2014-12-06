#ifndef SPHERE_H
#define SPHERE_H

#include "Vect.h"
#include "Color.h"
#include "Object.h"

using namespace std;

class Sphere : public Object {
	Vect center;
	double radius;
	Color color;
	
public:
	Sphere(); //defaults to x direction
	Sphere(Vect, double, Color);
	
	//method functions:
	Vect getCenter() { return center; }
	double getRadius() { return radius; }
	Color getColor() { return color; }
	
	virtual Vect getNormalAt(Vect point) {
		Vect normal = (point - center).normalized();
		return normal;
	}
	
	virtual double findIntersection(Ray ray) {
		Vect ray_origin = ray.getOrigin();
		Vect ray_direction = ray.getDirection();
		Vect sphere_center = center;
		Vect ominusc = ray_origin - sphere_center;
		
		double a = 1; // normalized
		double b = 2*(ominusc.vx())*ray_direction.vx() +
				   2*(ominusc.vy())*ray_direction.vy() +
				   2*(ominusc.vz())*ray_direction.vz();
		double c = pow(ominusc.vx(), 2) + pow(ominusc.vy(), 2) + pow(ominusc.vz(), 2) - (radius*radius);
		
		double d = b*b - 4*c;
		
		if (d > 0) {
			// the ray intersects the sphere
			
			// check a root to see if it's the one we want
			double root_a = (-1*b - sqrt(d))/2 - 0.00001;	//float to help with rounding
			
			if (root_a > 0) {
				//root a is the first intersection along the ray
				return root_a;
			}
			else {
				//root b is the first intersection along the ray
				return (sqrt(d) - b)/2 - 0.00001;	//float to help with rounding
			}
		}
		else {	//ray missed the sphere
			return -1;
		}
	}
};

Sphere::Sphere () {
	center = Vect(0, 0, 0);
	radius = 1.0;
	color = Color(0.5, 0.5, 0.5, 0);
}

Sphere::Sphere (Vect pos, double rad, Color col) {
	center = pos;
	radius = rad;
	color = col;
}

#endif