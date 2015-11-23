#ifndef SCENEGRAPH_H_
#define SCENEGRAPH_H_

#include "Node.h"
#include "Raytrace.h"

#ifdef __APPLE__
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

class SceneGraph {
public:
	SceneGraph();
	~SceneGraph();

	void reset(); // Reset everything

	void selectChild(); // Select the first child of the selected node
	void selectParent(); // Select the parent of the selected node
	void selectNextSibling(); // Select the previous sibling of the selected node
	void selectPrevSibling(); // Select the next sibling of the selected node
	void createNode(); // Create a new node as a child of the currently selected node
	void deleteNode(); // Erase the currently selected node, and all its children
	void deleteNode(Node *node); // Recursive deletion function to delete children
	void copyNode(); // Copy the currently selected node, and all its children
	void copyNode(Node *node); // Copy the attributes of the currently selected node
	void copyNode(Node *node, Node *copy); // Recursive copy function to copy children

	Node* getRootNode(); // Get root node
	Node* getSelectedNode(); // Get current node

	void updateMaterial(int material, Node *node); // Updates the material of the node and its children
	void updateTransformation(Node *node); // Updates the transformation matrix of each node
	void updateBoundingVolume(Node *node); // Updates the bounding volume of each node
	void updateGroupBoundingVolume(Node *current, Node *node); // Compute min/max values for group bounding box
	void updateBoundingVolumeType(); // Change the type of bounding volume
	void updateBoundingVolumeType(Node *node); // Recursively change the type of bounding volume

	void drawScene(); // Draw the scene
	void drawNode(Node *node); // Draw the node
	void drawBound(Node *node); // Draw bounding volume
	void drawBoundingVolume(Node *node); // Draw bounding volume
	void drawGroupBoundingVolume(Node *node); // Draw group axis-aligned bounding box

	void createTable(); // Create a table
	void createChair(); // Create a chair
	void createCabinet(); // Create a filing cabinet
	string createMesh(string filename); // Create a mesh object

	void renderScene(vec3 eyep, vec3 vdir, vec3 uvec, double fovy, bool setting[]); // Render the current scene
	void cacheScene(Node *node); // Caches the transformation matrices for rendering

private:
	Node *root; // Root node
	Node *selected; // Currently selected node
	
	int bv_type; // Bounding volume type

	vector<Material*> materials; // Material types

	double width, height; // Scene window width and height
	vec3 eyep; // Eye position
	vec3 vdir; // Viewing direction
	vec3 uvec; // World space up vector
	double fovy; // Total field of view in Y
	vec3 lpos; // Point light position
	vec3 lcol; // Point light color
	vec3 acol; // Ambient light color

	bool isRendering;
};

#endif