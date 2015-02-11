[Sample Raytrace Image]()

** GOALS COMPLETED: **

Advanced Rendering:
- Put the sphere inside a room-like space and render the 5 visible walls with different colors
- Put more than one light source of a different type
*In the scene there is a blue directional light pointing down and a red point light hovering someplace
*roughly between the two spheres and the camera.
- Cast sufficient rays so the image is correctly anti-aliased
*Can be changed by the 'aalevel' variable in main.cpp - uses pseudorandom scattering
- Cast shadows from the spheres onto the walls
- Make the sphere reflective and reflect the color of the wall
*Reflectivity determined by fourth 'special' variable in color class, 1.0 being perfectly reflective
*(use with rest of the colors black) and 0.0 being not reflective at all. A value of 2.0 causes checkered
*pattern on the ZX plane.
