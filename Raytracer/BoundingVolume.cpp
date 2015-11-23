#include "BoundingVolume.h"

BoundingVolume::BoundingVolume()
{
	bv_type = AABB; // AABB is default bounding type
	
	aabb_min_x = aabb_min_y = aabb_min_z = INF;
	aabb_max_x = aabb_max_y = aabb_max_z = -INF;

	obb_min_x = obb_min_y = obb_min_z = 1;
	obb_max_x = obb_max_y = obb_max_z = -1;

	bs_rad = -1; bs_x = 0; bs_y = 0; bs_z = 0;
	
	grp_aabb_min_x = grp_aabb_min_y = grp_aabb_min_z = INF;
	grp_aabb_max_x = grp_aabb_max_y = grp_aabb_max_z = -INF;
	
	grp_bs_rad = -1; grp_bs_x = 0; grp_bs_y = 0; grp_bs_z = 0;
}

BoundingVolume::~BoundingVolume()
{
}

void BoundingVolume::setBoundingType(int type)
{
	bv_type = type;
}

void BoundingVolume::setBoundingVolume(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z)
{
	switch (bv_type) {
	case AABB:	{
				aabb_min_x = min_x; aabb_max_x = max_x;
				aabb_min_y = min_y; aabb_max_y = max_y;
				aabb_min_z = min_z; aabb_max_z = max_z;
				break;
				}
	case OBB:	{
				obb_min_x = min_x; obb_max_x = max_x;
				obb_min_y = min_y; obb_max_y = max_y;
				obb_min_z = min_z; obb_max_z = max_z;
				break;
				}
	case BS:	{
				bs_rad = -1;
				break;
				}
	}
}

int BoundingVolume::getBoundingType()
{
	return bv_type;
}

void BoundingVolume::copyBoundingVolume(BoundingVolume *bv)
{
	bv_type = bv->bv_type;
	aabb_min_x = bv->aabb_min_x; aabb_max_x = bv->aabb_max_x;
	aabb_min_y = bv->aabb_min_y; aabb_max_y = bv->aabb_max_y;
	aabb_min_z = bv->aabb_min_z; aabb_max_z = bv->aabb_max_z;
	obb_min_x = bv->obb_min_x; obb_max_x = bv->obb_max_x;
	obb_min_y = bv->obb_min_y; obb_max_y = bv->obb_max_y;
	obb_min_z = bv->obb_min_z; obb_max_z = bv->obb_max_z;
	bs_rad = bv->bs_rad;
	bs_x = bv->bs_x;
	bs_y = bv->bs_y;
	bs_z = bv->bs_z;
	obb_t = bv->obb_t;
	grp_aabb_min_x = bv->grp_aabb_min_x; grp_aabb_max_x = bv->grp_aabb_max_x;
	grp_aabb_min_y = bv->grp_aabb_min_y; grp_aabb_max_y = bv->grp_aabb_max_y;
	grp_aabb_min_z = bv->grp_aabb_min_z; grp_aabb_max_z = bv->grp_aabb_max_z;
	grp_bs_rad = bv->grp_bs_rad; grp_bs_x = bv->grp_bs_x; grp_bs_y = bv->grp_bs_y; grp_bs_z = bv->grp_bs_z;
}

void BoundingVolume::computeAABB(int type, mat4 transformation, vector<vec3> &mesh_points)
{
	aabb_min_x = aabb_min_y = aabb_min_z = INF;
	aabb_max_x = aabb_max_y = aabb_max_z = -INF;
	
	// Apply transformation to the if node is nothing or sphere
	if (type == 0 || type == 1) {
		for (int ii = 0; ii < 6; ii++) {
			double pos_x = transformation[0][0]*sphereAABB[ii][0] + transformation[0][1]*sphereAABB[ii][1] + transformation[0][2]*sphereAABB[ii][2] + transformation[0][3]*sphereAABB[ii][3];
			double pos_y = transformation[1][0]*sphereAABB[ii][0] + transformation[1][1]*sphereAABB[ii][1] + transformation[1][2]*sphereAABB[ii][2] + transformation[1][3]*sphereAABB[ii][3];
			double pos_z = transformation[2][0]*sphereAABB[ii][0] + transformation[2][1]*sphereAABB[ii][1] + transformation[2][2]*sphereAABB[ii][2] + transformation[2][3]*sphereAABB[ii][3];
			double pos_k = transformation[3][0]*sphereAABB[ii][0] + transformation[3][1]*sphereAABB[ii][1] + transformation[3][2]*sphereAABB[ii][2] + transformation[3][3]*sphereAABB[ii][3];
			if (pos_k != 0) { pos_x /= pos_k; pos_y /= pos_k; pos_z /= pos_k; }
			if (pos_x < aabb_min_x) aabb_min_x = pos_x; if (pos_x > aabb_max_x) aabb_max_x = pos_x;
			if (pos_y < aabb_min_y) aabb_min_y = pos_y; if (pos_y > aabb_max_y) aabb_max_y = pos_y;
			if (pos_z < aabb_min_z) aabb_min_z = pos_z; if (pos_z > aabb_max_z) aabb_max_z = pos_z;
		}
	}
	// Apply transformation to the if node is cube
	else if (type == 2) {
		for (int ii = 0; ii < 8; ii++) {
			double pos_x = transformation[0][0]*cubeAABB[ii][0] + transformation[0][1]*cubeAABB[ii][1] + transformation[0][2]*cubeAABB[ii][2] + transformation[0][3]*cubeAABB[ii][3];
			double pos_y = transformation[1][0]*cubeAABB[ii][0] + transformation[1][1]*cubeAABB[ii][1] + transformation[1][2]*cubeAABB[ii][2] + transformation[1][3]*cubeAABB[ii][3];
			double pos_z = transformation[2][0]*cubeAABB[ii][0] + transformation[2][1]*cubeAABB[ii][1] + transformation[2][2]*cubeAABB[ii][2] + transformation[2][3]*cubeAABB[ii][3];
			double pos_k = transformation[3][0]*cubeAABB[ii][0] + transformation[3][1]*cubeAABB[ii][1] + transformation[3][2]*cubeAABB[ii][2] + transformation[3][3]*cubeAABB[ii][3];
			if (pos_k != 0) { pos_x /= pos_k; pos_y /= pos_k; pos_z /= pos_k; }
			if (pos_x < aabb_min_x) aabb_min_x = pos_x; if (pos_x > aabb_max_x) aabb_max_x = pos_x;
			if (pos_y < aabb_min_y) aabb_min_y = pos_y; if (pos_y > aabb_max_y) aabb_max_y = pos_y;
			if (pos_z < aabb_min_z) aabb_min_z = pos_z; if (pos_z > aabb_max_z) aabb_max_z = pos_z;
		}
	}
	// Apply transformation to the if node is cube
	else if (type == 3) {
		for (int ii = 0; ii < mesh_points.size(); ii++) {
			double pos_x = transformation[0][0]*mesh_points[ii][0] + transformation[0][1]*mesh_points[ii][1] + transformation[0][2]*mesh_points[ii][2] + transformation[0][3];
			double pos_y = transformation[1][0]*mesh_points[ii][0] + transformation[1][1]*mesh_points[ii][1] + transformation[1][2]*mesh_points[ii][2] + transformation[1][3];
			double pos_z = transformation[2][0]*mesh_points[ii][0] + transformation[2][1]*mesh_points[ii][1] + transformation[2][2]*mesh_points[ii][2] + transformation[2][3];
			double pos_k = transformation[3][0]*mesh_points[ii][0] + transformation[3][1]*mesh_points[ii][1] + transformation[3][2]*mesh_points[ii][2] + transformation[3][3];
			if (pos_k != 0) { pos_x /= pos_k; pos_y /= pos_k; pos_z /= pos_k; }
			if (pos_x < aabb_min_x) aabb_min_x = pos_x; if (pos_x > aabb_max_x) aabb_max_x = pos_x;
			if (pos_y < aabb_min_y) aabb_min_y = pos_y; if (pos_y > aabb_max_y) aabb_max_y = pos_y;
			if (pos_z < aabb_min_z) aabb_min_z = pos_z; if (pos_z > aabb_max_z) aabb_max_z = pos_z;
		}
	}
}

void BoundingVolume::computeOBB(int type, vector<vec3> &mesh_points)
{	
	// Apply transformation to the if node is nothing or sphere
	if (type == 0 || type == 1) {
		obb_min_x = obb_min_y = obb_min_z = sphereAABB[0][0];
		obb_max_x = obb_max_y = obb_max_z = sphereAABB[1][0];
	}
	// Apply transformation to the if node is cube
	else if (type == 2) {
		obb_min_x = obb_min_y = obb_min_z = cubeAABB[0][0];
		obb_max_x = obb_max_y = obb_max_z = cubeAABB[7][0];
	}
	// Apply transformation to the if node is cube
	else if (type == 3) {
		for (int ii = 0; ii < mesh_points.size(); ii++) {
			if (obb_min_x > mesh_points[ii][0]) obb_min_x = mesh_points[ii][0];
			if (obb_max_x < mesh_points[ii][0]) obb_max_x = mesh_points[ii][0];
			if (obb_min_y > mesh_points[ii][1]) obb_min_y = mesh_points[ii][1];
			if (obb_max_y < mesh_points[ii][1]) obb_max_y = mesh_points[ii][1];
			if (obb_min_z > mesh_points[ii][2]) obb_min_z = mesh_points[ii][2];
			if (obb_max_z < mesh_points[ii][2]) obb_max_z = mesh_points[ii][2];
		}
	}
}

void BoundingVolume::computeBS()
{
	bs_rad = -1; bs_x = 0; bs_y = 0; bs_z = 0;
	double max_rad = -INF;
	
	if (aabb_max_x - aabb_min_x > max_rad) max_rad = aabb_max_x - aabb_min_x;
	if (aabb_max_y - aabb_min_y > max_rad) max_rad = aabb_max_y - aabb_min_y;
	if (aabb_max_z - aabb_min_z > max_rad) max_rad = aabb_max_z - aabb_min_z;

	bs_rad = max_rad*0.5 + 0.5;
	bs_x = aabb_min_x + (aabb_max_x - aabb_min_x)*0.5;
	bs_y = aabb_min_y + (aabb_max_y - aabb_min_y)*0.5;
	bs_z = aabb_min_z + (aabb_max_z - aabb_min_z)*0.5;
}

void BoundingVolume::computeGrpAABB(double min_x, double max_x, double min_y, double max_y, double min_z, double max_z)
{
	if (grp_aabb_min_x > min_x) grp_aabb_min_x = min_x;
	if (grp_aabb_max_x < max_x) grp_aabb_max_x = max_x;
	if (grp_aabb_min_y > min_y) grp_aabb_min_y = min_y;
	if (grp_aabb_max_y < max_y) grp_aabb_max_y = max_y;
	if (grp_aabb_min_z > min_z) grp_aabb_min_z = min_z;
	if (grp_aabb_max_z < max_z) grp_aabb_max_z = max_z;
}

void BoundingVolume::computeGrpBS()
{
	grp_bs_rad = -1; grp_bs_x = 0; grp_bs_y = 0; grp_bs_z = 0;
	double max_rad = -INF;
	
	if (grp_aabb_max_x - grp_aabb_min_x > max_rad) max_rad = grp_aabb_max_x - grp_aabb_min_x;
	if (grp_aabb_max_y - grp_aabb_min_y > max_rad) max_rad = grp_aabb_max_y - grp_aabb_min_y;
	if (grp_aabb_max_z - grp_aabb_min_z > max_rad) max_rad = grp_aabb_max_z - grp_aabb_min_z;

	grp_bs_rad = max_rad*0.7; // So edges of cubes are not clipped off
	grp_bs_x = grp_aabb_min_x + (grp_aabb_max_x - grp_aabb_min_x)*0.5;
	grp_bs_y = grp_aabb_min_y + (grp_aabb_max_y - grp_aabb_min_y)*0.5;
	grp_bs_z = grp_aabb_min_z + (grp_aabb_max_z - grp_aabb_min_z)*0.5;
}

void BoundingVolume::computeBVHMatrix()
{
	double mat[16];

	glPushMatrix();
		glTranslated(grp_bs_x, grp_bs_y, grp_bs_z);
		glScaled(grp_bs_rad, grp_bs_rad, grp_bs_rad);
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
	
		bvh_t[0][0] = mat[0]; bvh_t[0][1] = mat[4]; bvh_t[0][2] = mat[8]; bvh_t[0][3] = mat[12];
		bvh_t[1][0] = mat[1]; bvh_t[1][1] = mat[5]; bvh_t[1][2] = mat[9]; bvh_t[1][3] = mat[13];
		bvh_t[2][0] = mat[2]; bvh_t[2][1] = mat[6]; bvh_t[2][2] = mat[10]; bvh_t[2][3] = mat[14];
		bvh_t[3][0] = mat[3]; bvh_t[3][1] = mat[7]; bvh_t[3][2] = mat[11]; bvh_t[3][3] = mat[15];

		bvh_t_inv = bvh_t.inverse();
		bvh_tstar = bvh_t;
		bvh_tstar[0][3] = 0.0; bvh_tstar[1][3] = 0.0; bvh_tstar[2][3] = 0.0;
		bvh_tstar_inv = bvh_tstar.inverse();
		bvh_t_trans_inv = bvh_t.transpose().inverse();
	glPopMatrix();
}

void BoundingVolume::resetBoundingVolume()
{
	aabb_min_x = aabb_min_y = aabb_min_z = INF;
	aabb_max_x = aabb_max_y = aabb_max_z = -INF;
	obb_min_x = obb_min_y = obb_min_z = INF;
	obb_max_x = obb_max_y = obb_max_z = -INF;
	bs_rad = -1; bs_x = 0; bs_y = 0; bs_z = 0;
	grp_aabb_min_x = grp_aabb_min_y = grp_aabb_min_z = INF;
	grp_aabb_max_x = grp_aabb_max_y = grp_aabb_max_z = -INF;
	grp_bs_rad = -1; grp_bs_x = 0; grp_bs_y = 0; grp_bs_z = 0;
}