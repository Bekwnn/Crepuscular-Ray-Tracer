#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Ray.h"
#include "Camera.h"
#include "Vect.h"
#include "Color.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"

using namespace std;

struct RGBType {
	double r;
	double g;
	double b;
};

/*performs 4(3) bit shifts on ints and casts them to unsigned char
 *in 4 subsequent addresses in a header */
void setfourblock(int data, unsigned char *header) {
	for (int i = 0; i < 4; i++) {
		*(header + i) = (unsigned char)(data>>(i*8));
	}
}

/* returns the index of which object in the intersections
 * should be rendered */
int RenderedObjectIndex(vector<double> object_intersections) {
	//return the index of the minimum value
	int index_of_minimum_value;
	
	//check case of size 0 and size 1:
	if (object_intersections.size() == 0) {
		return -1;
	}
	else if (object_intersections.size() == 1) {
		// if single intersection is > 0 then return it
		if (object_intersections.at(0) > 0) {
			return 0;
		}
		// otherwise the ray missed everything
		else {
			return -1;
		}
	}
	// find minimum non-negative value in intersections:
	else {
		double min = numeric_limits<double>::max();
		int index_of_min = -1;
		for (int i = 0; i < object_intersections.size(); i++) {
			if (min > object_intersections.at(i) && object_intersections.at(i) > 0) {
				min = object_intersections.at(i);
				index_of_min = i;
			}
		}
		return index_of_min;
	}
}

/* saves a bitmap */
void savebmp (const char *filename, int w, int h, int dpi, RGBType *data) {
	FILE *f;
	int k = w*h;
	int s = 4*k;
	int filesize = 54 + s;
	
	double factor = 39.375;
	int m = static_cast<int>(factor);
	
	int ppm = dpi*m;
	
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};
	
	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize>>8);
	bmpfileheader[4] = (unsigned char)(filesize>>16);
	bmpfileheader[5] = (unsigned char)(filesize>>24);
	
	//set some similar four address blocks by bitshifting ints
	setfourblock(w, &bmpinfoheader[4]);
	setfourblock(h, &bmpinfoheader[8]);
	setfourblock(s, &bmpinfoheader[21]);
	setfourblock(ppm, &bmpinfoheader[25]);
	setfourblock(ppm, &bmpinfoheader[29]);
	
	f = fopen(filename, "wb");
	
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
	
	for (int i = 0; i < k; i++) {
		RGBType rgb = data[i];
		
		double red = (data[i].r)*255;
		double green = (data[i].g)*255;
		double blue = (data[i].b)*255;
		
		unsigned char color[3] = {(int)floor(blue), (int)floor(green), (int)floor(red)};
		
		fwrite(color,1,3,f);
	}
	
	fclose(f);
}

int curpixel;

int main(int args, char *argv[]) {
	cout << "Rendering..." << endl;
	
	int dpi = 72;
	int width = 640;
	int height = 480;
	int pixelcount = width*height;
	RGBType *pixels = new RGBType[pixelcount];
	
	double aspectratio = (double)width/height;
	
	//establishing base vectors
	Vect O(0, 0, 0);
	Vect X(1, 0, 0);
	Vect Y(0, 1, 0);
	Vect Z(0, 0, 1);
	
	//set up camera:
	Vect campos(3, 1.5, -4);
	Vect look_at(0, 0, 0);
	Vect camlookdiff = campos - look_at;
	Vect camdir = (-camlookdiff).normalized();
	Vect camright = Y.cross(camdir).normalized();
	Vect camdown = camright.cross(camdir);
	Camera scene_cam(campos, camdir, camright, camdown);
	
	//establish some basic colors:
	Color white(1.0, 1.0, 1.0, 0);
	Color gray(0.5, 0.5, 0.5, 0);
	Color black(0, 0, 0, 0);
	
	//set up scene light(s):
	Vect light_position(-7, 10, -10);
	Light scene_light(light_position, white);
	
	//scene objects:
	Sphere sphere(O, 1, Color(0.2, 0.8, 0.2, 0));
	Plane scene_plane(Y, -1, Color(0.6, 0.1, 0.3, 0));
	
	//push scene objects to scene graph:
	vector<Object*> scene_graph;
	scene_graph.push_back(dynamic_cast<Object*>(&sphere));
	scene_graph.push_back(dynamic_cast<Object*>(&scene_plane));
	
	double xamnt, yamnt;
	
	//pixel rendering:
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			curpixel = y*width + x;
			
			//start with no AA
			if (width > height) {
				//the image is wider than it is tall
				xamnt = ((x+0.5)/width)*aspectratio - (((width-height)/(double)height/2));
				yamnt = ((height - y) + 0.5)/height;
			}
			else if (height > width) {
				//the image is taller than it is wide
				xamnt = (x + 0.5)/width;
				yamnt = (((height - y) + 0.5)/height)/aspectratio - (((height-width)/(double)width/2));
			}
			else {
				//image is 1:1 aspect ratio
				xamnt = (x + 0.5)/width;
				yamnt = ((height - y) + 0.5)/height;
			}
			
			Vect cam_ray_origin = scene_cam.getCameraPosition();
			Vect cam_ray_direction = (camdir + (camright * (xamnt - 0.5) + camdown * (yamnt - 0.5))).normalized();
			
			Ray cam_ray(cam_ray_origin, cam_ray_direction);
			
			vector<double> intersections;
			
			//loop through each object in the scene for intersections
			for (int i = 0; i < scene_graph.size(); i++) {
				intersections.push_back(scene_graph.at(i)->findIntersection(cam_ray));
			}
			
			int index_of_rendered_object = RenderedObjectIndex(intersections);
			
			if (index_of_rendered_object == -1) {
				//background color
				pixels[curpixel].r = 0.0;
				pixels[curpixel].g = 0.0;
				pixels[curpixel].b = 0.0;
			}
			else {
				//get intersected object color
				Color returnColor = scene_graph.at(index_of_rendered_object)->getColor();
				pixels[curpixel].r = returnColor.red();
				pixels[curpixel].g = returnColor.green();
				pixels[curpixel].b = returnColor.blue();
			}
		}
	}
	
	savebmp("scene.bmp", width, height, dpi, pixels);
	
	return 0;
}