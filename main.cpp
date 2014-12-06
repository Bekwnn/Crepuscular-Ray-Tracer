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
#include "PointLight.h"
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

/* gets the color at an intersection */
Color getColorAt(Vect intersection_position, Vect intersecting_ray_direction,
				 vector<Object*> scene_graph, int index_of_rendered_object,
				 vector<Light*> scene_lights, double rounderror, double ambientlight)
{
	Color rendered_object_color = scene_graph.at(index_of_rendered_object)->getColor();
	Vect rendered_object_normal = scene_graph.at(index_of_rendered_object)->getNormalAt(intersection_position);
	
	Color final_color = rendered_object_color * ambientlight;
	
	for (int i = 0; i < scene_lights.size(); i++) {
		Vect light_direction = (scene_lights.at(i)->getPosition() - intersection_position).normalized();
		
		double cos_angle = rendered_object_normal.normalized().dot(light_direction.normalized());
		
		if (cos_angle > 0) {
			bool shadowed = false;
			
			double distance_to_light = (scene_lights.at(i)->getPosition() - intersection_position).normalized().magnitude();
			
			Ray shadow_feeler(intersection_position, (scene_lights.at(i)->getPosition() - intersection_position).normalized());
			
			vector<double> secondary_intersections;
			
			for (int j = 0; j < scene_graph.size(); j++) {
				secondary_intersections.push_back(scene_graph.at(j)->findIntersection(shadow_feeler));
			}
			
			for (int k = 0; k < secondary_intersections.size(); k++) {
				if (secondary_intersections.at(k) > rounderror) {
					if (secondary_intersections.at(k) <= distance_to_light) {
						shadowed = true;
					}
					break;
				}
			}
			
			if (!shadowed) {
					final_color = final_color + ((rendered_object_color * scene_lights.at(i)->getColor()) * cos_angle); 
			
				if (rendered_object_color.special() <= 1) {
					double dot1 = rendered_object_normal.dot(-intersecting_ray_direction);
					Vect scalar1 = rendered_object_normal * dot1;
					Vect add1 = scalar1 + intersecting_ray_direction;
					Vect scalar2 = add1 * 2;
					Vect add2 = scalar2 - intersecting_ray_direction;
					Vect reflection_direction = add2.normalized();
					
					double specular = reflection_direction.dot(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color + (scene_lights.at(i)->getColor() * specular * rendered_object_color.special());
					}
				}
			}
		}
	}
	return final_color.clip();
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
	double ambientlight = 0.2;
	double rounderror = 0.000001;
	
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
	PointLight scene_light(light_position, white);
	//Vect light_position2(7, 10, -10);
	//PointLight scene_light2(light_position2, white);
	
	//push lights to a light source vector:
	vector<Light*> scene_lights;
	scene_lights.push_back(dynamic_cast<Light*>(&scene_light));
	//scene_lights.push_back(dynamic_cast<Light*>(&scene_light2));
	
	//scene objects:
	Sphere sphere(O, 1, Color(0.9, 0.6, 0.1, 0));
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
				if (intersections.at(index_of_rendered_object) > rounderror) {
				
					Vect intersection_position = cam_ray_origin +
						(cam_ray_direction * intersections.at(index_of_rendered_object));
					Vect intersecting_ray_direction = cam_ray_direction;
					
					Color intersection_color = getColorAt(
						intersection_position, intersecting_ray_direction,
						scene_graph, index_of_rendered_object, scene_lights,
						rounderror, ambientlight);
					
					pixels[curpixel].r = intersection_color.red();
					pixels[curpixel].g = intersection_color.green();
					pixels[curpixel].b = intersection_color.blue();
				}
			}
		}
	}
	
	savebmp("trace.bmp", width, height, dpi, pixels);
	
	return 0;
}