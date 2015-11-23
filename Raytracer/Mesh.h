#ifndef MESH_H_
#define MESH_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
#include <vector>

#include "algebra3.h"

enum { EXTRUSION, SURFREV };

struct Triangle {
	vec3 points;
	vec3 normal;
};

class Mesh {
public:
	Mesh();
	Mesh(string filename);
	~Mesh();

	bool fileParser(string filename); // Reads from input file
	void randomMesh(string mesh_type); // Generate a random mesh

	void generateMesh(); // Creates the triangles for the mesh
	void copyMesh(Mesh *mesh); // Copy mesh
	
	double getHeight(); // Get height
	bool getChecksum(); // Get checksum
	string getStatusMsg(); // Get status msg

	bool isConvex(); // Check if the extrusion base is convex

private:
	int mesh_type; // To check if it's extrusion or surfrev
	double extrude_length; // Extrustion length
	int radial_slice; // Number of radial slices
	int num_of_point; // Number of points

	double height; // Mesh object height
	bool checksum; // Check if the mesh was created successfully
	string status_msg; // Error message
	
	vector<vec3> points; // A vector to store the points
	vector<Triangle*> triangles; // A vector to store the points
	
	friend class Node;
	friend class Raytrace;
	friend class SceneGraph;
};

#endif