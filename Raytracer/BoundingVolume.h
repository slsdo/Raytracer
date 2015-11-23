#ifndef BOUNDINGVOLUME_H_
#define BOUNDINGVOLUME_H_

#include <vector>

#include "algebra3.h"

#ifdef __APPLE__
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

#define NUM_OF_BVTYPE 3
enum { NONE, AABB, OBB, BS };

// AABB points for NONE and SPHERE
const double sphereAABB[6][4] = {
{ 1.0, 0.0, 0.0, 1.0},
{-1.0, 0.0, 0.0, 1.0},
{ 0.0, 1.0, 0.0, 1.0},
{ 0.0,-1.0, 0.0, 1.0},
{ 0.0, 0.0, 1.0, 1.0},
{ 0.0, 0.0,-1.0, 1.0}};

// AABB points for CUBE
const double cubeAABB[8][4] = {
{ 0.6, 0.6, 0.6, 1.0},
{ 0.6, 0.6,-0.6, 1.0},
{ 0.6,-0.6, 0.6, 1.0},
{ 0.6,-0.6,-0.6, 1.0},
{-0.6, 0.6, 0.6, 1.0},
{-0.6, 0.6,-0.6, 1.0},
{-0.6,-0.6, 0.6, 1.0},
{-0.6,-0.6,-0.6, 1.0}};

class BoundingVolume {
public:
	BoundingVolume();
	~BoundingVolume();

	void setBoundingType(int type); // Set type of bounding volume
	void setBoundingVolume(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z); // Set values of bounding volume
	
	int getBoundingType(); // Get type of bounding volume

	void copyBoundingVolume(BoundingVolume *bv); // Copy bounding volume

	void computeAABB(int type, mat4 transformation, vector<vec3> &mesh_points); // Compute min/max values for axis-aligned bounding box
	void computeOBB(int type, vector<vec3> &mesh_points); // Compute min/max values for oriented bounding box
	void computeBS(); // Compute radius for bounding sphere
	void computeGrpAABB(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z); // Compute min/max values for group axis-aligned bounding box
	void computeGrpBS(); // Compute radius for group bounding sphere
	void computeBVHMatrix(); // Compute transformation matrix for BVH

	void resetBoundingVolume(); // Reset everything

private:
	int bv_type; // Type of bounding volume

	double aabb_min_x, aabb_max_x, aabb_min_y, aabb_max_y, aabb_min_z, aabb_max_z; // Min/max values for AABB
	double obb_min_x, obb_max_x,obb_min_y, obb_max_y, obb_min_z, obb_max_z; // Min/max values for OBB
	double bs_rad, bs_x, bs_y, bs_z; // Radius and translation for bounding sphere
	mat4 obb_t; // Transformation matrix for OBB
	double grp_aabb_min_x, grp_aabb_max_x, grp_aabb_min_y, grp_aabb_max_y, grp_aabb_min_z, grp_aabb_max_z; // Min/max values for group AABB
	double grp_bs_rad, grp_bs_x, grp_bs_y, grp_bs_z; // Radius and translation for group bounding sphere	

	mat4 bvh_t; // Transformation matrix
	mat4 bvh_t_inv; // Inverse of transformation matrix
	mat4 bvh_tstar; // Transformation ignoring the translation component
	mat4 bvh_tstar_inv; // Inverse of transformation ignoring the translation component
	mat4 bvh_t_trans_inv; // Transposed inverse of transformation
	
	friend class Node;
	friend class SceneGraph;
	friend class Raytrace;
};

#endif