#ifndef NODE_H_
#define NODE_H_

#include "Mesh.h"
#include "BoundingVolume.h"

enum { NOTHING, SPHERE, CUBE, MESH };
enum { MAT_DARK, MAT_RED, MAT_GREEN, MAT_BLUE, MAT_GLASS };

struct Material {
	double r, g, b; // RGB color value
	double specular;
	double reflectivity;
	double transparency;
	double refraction;
};

class Node {
public:
	Node();
	~Node();

	void setTranslation(double x, double y, double z); // Change the node's translation
	void setRotation(double x, double y, double z); // Change the node's rotation
	void setScaling(double x, double y, double z); // Change the node's scale
	void setTransformation(double mat[]); // Change the node's transformation
	void setType(int type); // Change the currently selected node to draw an object
	void setMaterial(int material); // Change the material of the currently selected node
	void setBoundType(int bv); // Change the type of bounding volume

	vec3 getTranslation(); // Return node translation
	vec3 getRotation(); // Return node rotation
	vec3 getScaling(); // Return node scaling
	int getType(); // Return node object type
	int getMaterial(); // Return node material type
	int getBvType(); // Return bounding volume type
	bool getMeshChecksum(); // Check if the node has a mesh object

	// Node translation, rotation, and scaling
	void incTranslationX();	void decTranslationX();
	void incTranslationY();	void decTranslationY();
	void incTranslationZ();	void decTranslationZ();
	void incRotationX();	void decRotationX();
	void incRotationY();	void decRotationY();
	void incRotationZ();	void decRotationZ();
	void incScalingX();		void decScalingX();
	void incScalingY();		void decScalingY();
	void incScalingZ();		void decScalingZ();

	void computeBoundingVolume(); // Compute bounding volume
	void computeGroupBoundingVolume(Node *node);
	void computeTransformations(); // Compute transformation matrices for raytracing

private:
	double x_trans, y_trans, z_trans; // Translation
	double x_rot, y_rot, z_rot; // Rotation
	double x_scal, y_scal, z_scal; // Scaling

	int type; // Primitive type
	int material; // Material type

	Node *parent; // Pointer to parent node
	Node *child; // Pointer to child node
	Node *next_sibling; // Pointer to right sibling
	Node *prev_sibling; // Pointer to left sibling

	Mesh *mesh; // Mesh object
	BoundingVolume *bound; // Bounding volume

	mat4 t; // Transformation matrix
	mat4 t_inv; // Inverse of transformation matrix
	mat4 tstar; // Transformation ignoring the translation component
	mat4 tstar_inv; // Inverse of transformation ignoring the translation component
	mat4 t_trans_inv; // Transposed inverse of transformation

	friend class Raytrace;
	friend class SceneGraph;
};

#endif