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
	
	double brightness() {
		return (r + g + b)/3;
	}
	
	Color clip() {
		double alllight = r + g + b;
		double excesslight = alllight - 3;
		if (excesslight > 0) {
			r = r + excesslight*(r/alllight);
			g = g + excesslight*(g/alllight);
			b = b + excesslight*(b/alllight);
		}
		
		if (r > 1) { r = 1; }
		if (g > 1) { g = 1; }
		if (b > 1) { b = 1; }
		
		if (r < 0) { r = 0; }
		if (g < 0) { g = 0; }
		if (b < 0) { b = 0; }
		
		return Color(r, g, b, s);
	}
	
	// Operators:
	inline Color operator +(const Color &c) const {
		return Color(r + c.r, g + c.g, b + c.b, s);
	}
	
	inline Color& operator +=(const Color &c) {
		r += c.r;
		g += c.g;
		b += c.b;
		return (*this);
	}
	
	inline Color operator -(const Color &c) const {
		return Color(r - c.r, g - c.g, b - c.b, s);
	}
	
	inline Color& operator -=(const Color &c) {
		r -= c.r;
		g -= c.g;
		b -= c.b;
		return (*this);
	}
	
	inline Color operator *(double scalar) {
		return Color(r*scalar, g*scalar, b*scalar, s);
	}
	
	inline Color& operator *=(double scalar) {
		r *= scalar;
		g *= scalar;
		b *= scalar;
		return (*this);
	}
	
	inline Color operator *(const Color &c) const {
		return Color(r * c.r, g * c.g, b * c.b, s);
	}
	
	inline Color& operator *=(const Color &c) {
		r *= c.r;
		g *= c.g;
		b *= c.b;
		return (*this);
	}
	
	inline Color operator /(double scalar) {
		return Color(r/scalar, g/scalar, b/scalar, s);
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