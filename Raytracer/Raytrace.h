#ifndef RAYTRACE_H
#define RAYTRACE_H

#include "math.h"
#include <vector>

#include "algebra3.h"
#include "Node.h"
#include "Intersection.h"
#include "SceneGraph.h"

#ifdef __APPLE__
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif
#include <SOIL/SOIL.h>

const int max_depth = 5;

struct Ray
{
	vec3 o; // Ray origin
	vec3 d; // Ray direction
	double dist; // Distance from origin to intersection
};

class Raytrace {
public:
	Raytrace();
	Raytrace(double width, double height, vector<Material*> mat, Node* root, bool setting[]);
	~Raytrace();
	
	void initLighting(vec3 lpos, vec3 lcol, vec3 acol); // Set up the lights
	void initCamera(vec3 eyep, vec3 vdir, vec3 uvec, double fovy); // Set up the camera for rendering
	Ray* initRay(vec3 o, vec3 dir); // Create a single ray
	void initRays(); // Set up the rays for casting

	void render(); // The main raytracing methods
	void renderProgress(); // Displays progress bar

	void traceRay(Ray *ray, int depth, vec3 &color); // Trace a ray

	// Lighting/ray equations
	void computeDiffuseAmbient(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color);
	void computeTransparency(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color);
	void computeSpecular(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color);
	void computeReflection(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color);
	void computeRefraction(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color);
	void computeFresnelReflection(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color);
	double rSchlick2(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color);

	bool shadowRayUnblocked(vec3 intersection, vec3 l); // Check for shadows
	bool insideObject(vec3 normal, Ray *ray); // Check if ray is inside an object	
	void hierarchicalBoundingVolumes(Ray *ray, Node* node, double &tmin, vec3 &nmin, Node* &selected); // Check for intersection
	double rayIntersect(Ray *ray, Node* node, vec3 &nmin); // Check for intersection

private:
	double reso_x, reso_y; // Screen resolution
	vec3 midpoint, h_px, v_px; // Midpoint, width and height of screen in terms of pixel
	vec3 eyep; // Eye position
	vec3 vdir; // Viewing direction
	vec3 uvec; // World space up vector
	double fovy; // Total field of view in Y
	vec3 lpos; // Light position
	vec3 lcol; // Light color
	vec3 acol; // Ambient light color

	Node *root; // Root node
	vector<Material*> materials; // Materials

	bool *render_setting; // Render setting

	int progress_bar;
	bool progress_flag;
};

#endif