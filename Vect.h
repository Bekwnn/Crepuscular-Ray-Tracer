#ifndef VECT_H
#define VECT_H

#include "math.h"

class Vect {
	double x, y, z;
	
public:
	Vect();
	Vect(double, double, double);
	
	//method functions:
	double vx() { return x; }
	double vy() { return y; }
	double vz() { return z; }
	
	double magnitude () {
		return sqrt(x*x + y*y + z*z);
	}
	
	Vect normalized() {
		double magnitude = sqrt(x*x + y*y + z*z);
		return Vect(x/magnitude, y/magnitude, z/magnitude);
	}
	
	double dot(Vect v) {
		return x*v.vx() + y*v.vy() + z*v.vz();
	}
	
	Vect cross(Vect v) {
		return Vect (
			y*v.vz() - z*v.vy(),
			z*v.vx() - x*v.vz(),
			x*v.vy() - y*v.vx()
		);
	}
	
	// Operators:
	inline Vect operator -(void) const {
		return Vect(-x, -y, -z);
	}
	
	inline Vect operator +(const Vect &v) const {
		return Vect(x + v.x, y + v.y, z + v.z);
	}
	
	inline Vect& operator +=(const Vect &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return (*this);
	}
	
	inline Vect operator -(const Vect &v) const {
		return Vect(x - v.x, y - v.y, z - v.z);
	}
	
	inline Vect& operator -=(const Vect &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return (*this);
	}
	
	inline Vect operator *(double scalar) {
		return Vect(x*scalar, y*scalar, z*scalar);
	}
	
	inline Vect& operator *=(double scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return (*this);
	}
	
	inline Vect operator *(const Vect &v) const {
		return Vect(x * v.x, y * v.y, z * v.z);
	}
	
	inline Vect& operator *=(const Vect &v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return (*this);
	}
};

Vect::Vect () {
	x = 0;
	y = 0;
	z = 0;
}

Vect::Vect (double i, double j, double k) {
	x = i;
	y = j;
	z = k;
}

#endif