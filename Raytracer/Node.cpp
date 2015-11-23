#include "Node.h"

Node::Node()
{
	type = NOTHING; // Default is NOTHING
	material = MAT_DARK; // Default is GREY

	// Initialize translation, rotation, scaling
	x_trans = y_trans = z_trans = 0.0;
	x_rot = y_rot = z_rot = 0.0;
	x_scal = y_scal = z_scal = 1.0;

	mesh = new Mesh();
	bound = new BoundingVolume();
}

Node::~Node()
{
	delete mesh;
	delete bound;
}

void Node::setTranslation(double x, double y, double z)
{
	x_trans = x;
	y_trans = y;
	z_trans = z;
}

void Node::setRotation(double x, double y, double z)
{
	x_rot = x;
	y_rot = y;
	z_rot = z;
}

void Node::setScaling(double x, double y, double z)
{
	x_scal = x;
	y_scal = y;
	z_scal = z;
}

void Node::setTransformation(double mat[])
{
	t[0][0] = mat[0]; t[0][1] = mat[4]; t[0][2] = mat[8]; t[0][3] = mat[12];
	t[1][0] = mat[1]; t[1][1] = mat[5]; t[1][2] = mat[9]; t[1][3] = mat[13];
	t[2][0] = mat[2]; t[2][1] = mat[6]; t[2][2] = mat[10]; t[2][3] = mat[14];
	t[3][0] = mat[3]; t[3][1] = mat[7]; t[3][2] = mat[11]; t[3][3] = mat[15];
}

void Node::setType(int tp)
{
	switch (tp) {
		case NOTHING:	type = NOTHING; break;
		case SPHERE:	type = SPHERE; break;
		case CUBE:		type = CUBE; break;
		case MESH:		type = MESH; break;
		default:		break;
	}
}

void Node::setMaterial(int mat)
{
	switch (mat) {
		case MAT_DARK:		material = MAT_DARK; break;
		case MAT_RED:		material = MAT_RED; break;
		case MAT_GREEN:		material = MAT_GREEN; break;
		case MAT_BLUE:		material = MAT_BLUE; break;
		case MAT_GLASS:		material = MAT_GLASS; break;
		default:			break;
	}
}

void Node::setBoundType(int bv)
{
	bound->setBoundingType(bv);
}

vec3 Node::getTranslation() { return vec3(x_trans, y_trans, z_trans); }
vec3 Node::getRotation() { return vec3(x_rot, y_rot, z_rot); }
vec3 Node::getScaling() { return vec3(x_scal, y_scal, z_scal); }
int Node::getType() { return type; }
int Node::getMaterial() { return material; }
int Node::getBvType() { return bound->getBoundingType(); }
bool Node::getMeshChecksum() { return mesh->getChecksum(); }

void Node::incTranslationX() { x_trans += 1.0; }
void Node::decTranslationX() { x_trans -= 1.0; }
void Node::incTranslationY() { y_trans += 1.0; }
void Node::decTranslationY() { y_trans -= 1.0; }
void Node::incTranslationZ() { z_trans += 1.0; }
void Node::decTranslationZ() { z_trans -= 1.0; }
void Node::incRotationX() { x_rot += 10.0; }
void Node::decRotationX() { x_rot -= 10.0; }
void Node::incRotationY() { y_rot += 10.0; }
void Node::decRotationY() { y_rot -= 10.0; }
void Node::incRotationZ() { z_rot += 10.0; }
void Node::decRotationZ() { z_rot -= 10.0; }
void Node::incScalingX() { x_scal += 0.2; }
void Node::decScalingX() { x_scal -= 0.2; }
void Node::incScalingY() { y_scal += 0.2; }
void Node::decScalingY() { y_scal -= 0.2; }
void Node::incScalingZ() { z_scal += 0.2; }
void Node::decScalingZ() { z_scal -= 0.2; }

void Node::computeBoundingVolume()
{
	bound->computeAABB(type, t, mesh->points);
	bound->computeOBB(type, mesh->points);
	bound->computeBS();
}

void Node::computeGroupBoundingVolume(Node *node)
{
	bound->computeGrpAABB(node->bound->aabb_min_x, node->bound->aabb_max_x,
						  node->bound->aabb_min_y, node->bound->aabb_max_y,
						  node->bound->aabb_min_z, node->bound->aabb_max_z);
	bound->computeGrpBS();
}

void Node::computeTransformations()
{
	t_inv = t.inverse();
	tstar = t;
	tstar[0][3] = 0.0; tstar[1][3] = 0.0; tstar[2][3] = 0.0;
	tstar_inv = tstar.inverse();
	t_trans_inv = t.transpose().inverse();
}