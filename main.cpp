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
#include "DirectionalLight.h"
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
 *in 4 subsequent addresses in a header - Used for file creation */
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
	
	//check case of size 0 and size 1 to speed things up a bit:
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
				 vector<Light*> scene_lights, double ambientlight,
				 int recursion_step)
{
	double rounderror = 0.000001;	//used at some intersections
	
	Color rendered_object_color = scene_graph.at(index_of_rendered_object)->getColor();
	Vect rendered_object_normal = scene_graph.at(index_of_rendered_object)->getNormalAt(intersection_position);
	
	if (rendered_object_color.special() == 2) {
		//checkered pattern
		
		int square = floor(intersection_position.vx()) + floor(intersection_position.vz());
		if (square % 2 == 0) {
			//darker tile
			rendered_object_color *= 0.5;
		}
		else {
			//do nothing
		}
	}
	
	Color final_color = rendered_object_color * ambientlight;
	
	int recursion_limit = 20;
	
	//special value of (0-1] for reflection:
	if (rendered_object_color.special() > 0 && rendered_object_color.special() <= 1 && recursion_step < recursion_limit) {
		//reflection of objects with specular:
		double dot1 = rendered_object_normal.dot(-intersecting_ray_direction);
		Vect scalar1 = rendered_object_normal * dot1;
		Vect add1 = scalar1 + intersecting_ray_direction;
		Vect scalar2 = add1 * 2;
		Vect add2 = scalar2 - intersecting_ray_direction;
		Vect reflection_direction = add2.normalized();
		
		Ray reflection_feeler(intersection_position, reflection_direction);
		
		vector<double> reflection_intersections;
		
		for (int i = 0; i < scene_graph.size(); i++) {
			reflection_intersections.push_back(scene_graph.at(i)->findIntersection(reflection_feeler));
		}
		
		int index_of_reflected_object = RenderedObjectIndex(reflection_intersections);
		
		if (index_of_reflected_object != -1) {
			//nothing to reflect:
			if (reflection_intersections.at(index_of_reflected_object) > rounderror) {
				Vect reflection_intersection_position = intersection_position + (reflection_direction * reflection_intersections.at(index_of_reflected_object));
				Vect reflection_intersection_ray_direction = reflection_direction;
				
				Color reflection_intersection_color = getColorAt(
					reflection_intersection_position, reflection_intersection_ray_direction,
					scene_graph, index_of_reflected_object, scene_lights, ambientlight,
					(recursion_step+1)
				);
				
				final_color = final_color + (reflection_intersection_color * rendered_object_color.special());
			}
		}
	}
	
	for (int i = 0; i < scene_lights.size(); i++) {
		Vect light_direction = (scene_lights.at(i)->getDirectionFrom(intersection_position)).normalized();
		
		double cos_angle = rendered_object_normal.normalized().dot(light_direction);
		
		if (cos_angle > 0) {
			bool shadowed = false;
			
			double distance_to_light = scene_lights.at(i)->getDistanceFrom(intersection_position);
			
			Ray shadow_feeler(intersection_position, (scene_lights.at(i)->getDirectionFrom(intersection_position)).normalized());
			
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
	
	//set some similar four address blocks by bitshifting 32ints into unsigned chars
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

int main(int args, char *argv[]) {
	cout << "Rendering..." << endl;
	
	clock_t t_start, t_end;
	t_start = clock();
	
	int dpi = 72;
	int width = 1600;
	int height = 900;
	int pixelcount = width*height;
	RGBType *pixels = new RGBType[pixelcount];
	
	int aalevel = 1;		//1 = no AA, 2 = 4 rays per pixel, 3 = 9, etc
	int aaseed = 424242;	//chosen arbitrarily
	
	double aspectratio = (double)width/height;
	double ambientlight = 0.2;
	double rounderror = 0.000001;
	
	//establish some basic colors:
	Color white(1.0, 1.0, 1.0, 0);
	Color gray(0.5, 0.5, 0.5, 0);
	Color black(0, 0, 0, 0);
	
	//establishing base vectors
	Vect Zero(0, 0, 0);
	Vect X(1, 0, 0);
	Vect Y(0, 1, 0);
	Vect Z(0, 0, 1);
	
	//set up camera:
	Vect camera_position(0, 3, -6);
	Vect look_at(0, 2, 0);
	Vect camlookdiff = camera_position - look_at;
	Vect camdir = (-camlookdiff).normalized();
	Vect camright = Y.cross(camdir).normalized();
	Vect camdown = camright.cross(camdir);
	Camera scene_cam(camera_position, camdir, camright, camdown);
	
	//create scene light(s):
	PointLight scene_light(Vect(0,2,-2), Color(0.8, 0.1, 0.5, 0));
	DirectionalLight sun(Vect(0,-1,0), Color(0.1, 0.1, 0.8, 0));
	
	//push lights to a light source vector:
	vector<Light*> scene_lights;
	scene_lights.push_back(dynamic_cast<Light*>(&scene_light));
	scene_lights.push_back(dynamic_cast<Light*>(&sun));
	
	//create scene objects:
	Sphere sphere(Vect(-1,0,0), 1, Color(0.8, 0.5, 0, 0.5));
	Sphere sphere2(Vect(2.5,0.5,2.5), 1, Color(0.3, 0.7, 0.7, 0.5));
	Plane bot_plane(Y, -1, Color(0.6, 0.1, 0.3, 2));
	Plane top_plane(-Y, -5, Color(0.6, 0.1, 0.3, 0));
	Plane back_plane(-Z, -5, Color(0.2, 0.6, 0.2, 0));
	Plane left_plane(-X, -5, Color(0, 0.4, 0.7, 0));
	Plane right_plane(X, -5, Color(0, 0.4, 0.7, 0));
	
	//push scene objects to scene graph:
	vector<Object*> scene_graph;
	scene_graph.push_back(dynamic_cast<Object*>(&sphere));
	scene_graph.push_back(dynamic_cast<Object*>(&sphere2));
	scene_graph.push_back(dynamic_cast<Object*>(&bot_plane));
	scene_graph.push_back(dynamic_cast<Object*>(&top_plane));
	scene_graph.push_back(dynamic_cast<Object*>(&back_plane));
	scene_graph.push_back(dynamic_cast<Object*>(&left_plane));
	scene_graph.push_back(dynamic_cast<Object*>(&right_plane));
	
	double xamount, yamount;
	
	int aalevel2 = aalevel*aalevel;
	srand(aaseed);	//set seed for pseudorandom AA ray scattering
	
	double AARed[aalevel2];
	double AAGreen[aalevel2];
	double AABlue[aalevel2];
	
	//pixel rendering:
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int curpixel = y*width + x;
			
			for(int aax = 0; aax < aalevel; aax++) {
				for(int aay = 0; aay < aalevel; aay++) {
					int thisaa_index = aax*aalevel + aay;
					
					//subpixel offsets, with pseudo-random sub-sub-pixel offsets:
					double aaxoffset = ((double)aax + 1)/(double)aalevel - 1/(double)(2*aalevel) + fmod(rand(),(1/(double)aalevel));
					double aayoffset = ((double)aay + 1)/(double)aalevel - 1/(double)(2*aalevel) + fmod(rand(),(1/(double)aalevel));
					
					if (width > height) {
						//the image is wider than it is tall
						xamount = ((x + aaxoffset)/width)*aspectratio - (((width-height)/(double)height/2));
						yamount = (height - y + aayoffset)/height;
					}
					else if (height > width) {
						//the image is taller than it is wide
						xamount = (x + aaxoffset)/width;
						yamount = ((height - y + aayoffset)/height)/aspectratio - (((height-width)/(double)width/2));
					}
					else {
						//image is 1:1 aspect ratio
						xamount = (x + aaxoffset)/width;
						yamount = (height - y + aayoffset)/height;
					}
					
					Vect cam_ray_origin = scene_cam.getCameraPosition();
					Vect cam_ray_direction = (camdir + (camright * (xamount - 0.5) + camdown * (yamount - 0.5))).normalized();
					
					Ray cam_ray(cam_ray_origin, cam_ray_direction);
					
					vector<double> intersections;
					
					//loop through each object in the scene for intersections
					for (int i = 0; i < scene_graph.size(); i++) {
						intersections.push_back(scene_graph.at(i)->findIntersection(cam_ray));
					}
					
					int index_of_rendered_object = RenderedObjectIndex(intersections);
					
					if (index_of_rendered_object == -1) {
						//define background color, if you wish:
						AARed[thisaa_index] = 0.0;
						AAGreen[thisaa_index] = 0.0;
						AABlue[thisaa_index] = 0.0;
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
								ambientlight, 0);
							
							AARed[thisaa_index] = intersection_color.red();
							AAGreen[thisaa_index] = intersection_color.green();
							AABlue[thisaa_index] = intersection_color.blue();
						}
					}
				} //end of AA inner loop
			} //end of AA outer loop
			
			//average the red AA rays:
			double red_average = 0.0;
			for (int r = 0; r < aalevel2; r++) {
				red_average += AARed[r];
			}
			red_average = red_average/aalevel2;
			
			//average the green AA rays:
			double green_average = 0.0;
			for (int g = 0; g < aalevel2; g++) {
				green_average += AAGreen[g];
			}
			green_average = green_average/aalevel2;
			
			//average the blue AA rays:
			double blue_average = 0.0;
			for (int b = 0; b < aalevel2; b++) {
				blue_average += AABlue[b];
			}
			blue_average = blue_average/aalevel2;
			
			pixels[curpixel].r = AARed[0];
			pixels[curpixel].g = AAGreen[0];
			pixels[curpixel].b = AABlue[0];
		} //end of pixel inner loop
	} //end of pixel outer loop
	
	//SET FILE NAME HERE, FILES WILL BE OVERWRITTEN BY THIS:
	savebmp("trace_NOAA.bmp", width, height, dpi, pixels);
	
	delete pixels, AARed, AAGreen, AABlue;
	
	t_end = clock();
	float render_time = ((float)t_end - (float)t_start)/1000;
	
	cout << render_time << " seconds to render." << endl;	//gives incorrect time on some operating systems
	
	return 0;
}