#ifndef COLOR_H
#define COLOR_H

#include "math.h"

class Color {
	double r, g, b, s;
	
public:
	Color();
	Color(double, double, double, double);
	
	//method functions:
	double red() { return r; }
	double green() { return g; }
	double blue() { return b; }
	double special() { return s; }
	void setR(double value) { r = value; }
	void setG(double value) { g = value; }
	void setB(double value) { b = value; }
	void setS(double value) { s = value; }
	
	// Operators:
	inline Color operator +(const Color &c) const {
		return Color(r + c.r, g + c.g, b + c.b, s + c.s);
	}
	
	inline Color& operator +=(const Color &c) {
		r += c.r;
		g += c.g;
		b += c.b;
		s += c.s;
		return (*this);
	}
	
	inline Color operator -(const Color &c) const {
		return Color(r - c.r, g - c.g, b - c.b, s - c.s);
	}
	
	inline Color& operator -=(const Color &c) {
		r -= c.r;
		g -= c.g;
		b -= c.b;
		s -= c.s;
		return (*this);
	}
	
	inline Color operator *(double scalar) {
		return Color(r*scalar, g*scalar, b*scalar, s*scalar);
	}
	
	inline Color& operator *=(double scalar) {
		r *= scalar;
		g *= scalar;
		b *= scalar;
		s *= scalar;
		return (*this);
	}
	
	inline Color operator *(const Color &c) const {
		return Color(r * c.r, g * c.g, b * c.b, s * c.s);
	}
	
	inline Color& operator *=(const Color &c) {
		r *= c.r;
		g *= c.g;
		b *= c.b;
		s *= c.s;
		return (*this);
	}
};

Color::Color () {
	r = 0.5;
	g = 0.5;
	b = 0.5;
	s = 0.0;
}

Color::Color (double red, double blue, double green, double special) {
	r = red;
	g = green;
	b = blue;
	s = special;
}

#endif