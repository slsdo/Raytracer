#include "SceneGraph.h"

SceneGraph::SceneGraph()
{
	root = new Node();

	root->child = NULL;
	root->parent = root;
	root->next_sibling = NULL;
	root->prev_sibling = NULL;

	selected = root; // Root is selected
	
	// Initialize group bounding box
	bv_type = AABB;

	// Initialize materials

	Material *material_dark = new Material();	
	material_dark->r = 0.2; material_dark->g = 0.2; material_dark->b = 0.2;
	material_dark->specular = 10.0; material_dark->reflectivity = 1.0;
	material_dark->transparency = 0.0; material_dark->refraction = 0.0;
	materials.push_back(material_dark);
	
	Material *material_red = new Material();	
	material_red->r = 1.0; material_red->g = 0.0; material_red->b = 0.0;
	material_red->specular = 10.0; material_red->reflectivity = 0.7;
	material_red->transparency = 0.0; material_red->refraction = 0.0;
	materials.push_back(material_red);
	
	Material *material_green = new Material();
	material_green->r = 0.0; material_green->g = 1.0; material_green->b = 0.0;
	material_green->specular = 10.0; material_green->reflectivity = 0.7;
	material_green->transparency = 0.0; material_green->refraction = 0.0;
	materials.push_back(material_green);
	
	Material *material_blue = new Material();
	material_blue->r = 0.0; material_blue->g = 0.0; material_blue->b = 1.0;
	material_blue->specular = 10.0; material_blue->reflectivity = 0.7;
	material_blue->transparency = 0.0; material_blue->refraction = 0.0;
	materials.push_back(material_blue);
	
	Material *material_glass = new Material();
	material_glass->r = 0.8; material_glass->g = 0.8; material_glass->b = 0.8;
	material_glass->specular = 0.0; material_glass->reflectivity = 0.0;
	material_glass->transparency = 1.0; material_glass->refraction = 1.52;
	materials.push_back(material_glass);
	
	// Initialize parameters for raytracers
	width = 640.0; height = 480.0;
	lpos = vec3(10.0, 12.0, 12.5);
	lcol = vec3(1.0, 1.0, 1.0);
	acol = vec3(0.2, 0.2, 0.2);
}

SceneGraph::~SceneGraph()
{
	delete root;
	delete selected;

	materials.clear();
}

void SceneGraph::reset()
{
	// Delete all children of root
	while (root->child != NULL) {
		selected = root->child;
		deleteNode();
	}
	
	root->x_trans = root->y_trans = root->z_trans = 0.0;
	root->x_rot = root->y_rot = root->z_rot = 0.0;
	root->x_scal = root->y_scal = root->z_scal = 1.0;
	root->bound->resetBoundingVolume();
	bv_type = AABB;
}

void SceneGraph::selectChild()
{
	if (selected->child != NULL)
		selected = selected->child;
}

void SceneGraph::selectParent()
{
	if (selected->parent != NULL)
		selected = selected->parent;
}

void SceneGraph::selectNextSibling()
{
	if (selected->next_sibling != NULL)
		selected = selected->next_sibling;
}

void SceneGraph::selectPrevSibling()
{
	if (selected->prev_sibling != NULL)
		selected = selected->prev_sibling;
}

void SceneGraph::createNode()
{
	Node *node = new Node();
	node->child = NULL;
	node->parent = selected;
	node->next_sibling = NULL;
	node->prev_sibling = NULL;
	node->bound->setBoundingType(bv_type);

	// If selected node has no children
	if (selected->child == NULL) {
		selected->child = node;
	}
	// If selected node has one or more children
	else {
		Node *temp = selected->child;
		while (temp->next_sibling != NULL)
			temp = temp->next_sibling;
		temp->next_sibling = node;
		node->prev_sibling = temp;
		
		temp = NULL;
		delete temp;
	}
	selected = node;
}

void SceneGraph::deleteNode()
{
	// Root node cannot be deleted
	if (selected != root) {
		Node *temp = selected->parent;
		deleteNode(selected);
		selected = temp;

		temp = NULL;
		delete temp;
	}
}

void SceneGraph::deleteNode(Node *node)
{
	// Recursively delete all children
	if (node->child != NULL)
		deleteNode(node->child);
	if (node != selected) {
		if (node->next_sibling != NULL)
			deleteNode(node->next_sibling);
	}
	// Delete node
	if (node->parent->child == node)
		node->parent->child = node->next_sibling;
	if (node->next_sibling != NULL)
		node->next_sibling->prev_sibling = node->prev_sibling;
	if (node->prev_sibling != NULL)
		node->prev_sibling->next_sibling = node->next_sibling;
	
	delete node;
}

void SceneGraph::copyNode()
{
	// Root node cannot be copied
	if (selected != root) {
		Node *copy = selected;
		copyNode(selected->parent, copy);

		copy = NULL;
		delete copy;
	}
}

void SceneGraph::copyNode(Node *node)
{
	selected->x_trans = node->x_trans;
	selected->y_trans = node->y_trans;
	selected->z_trans = node->z_trans;
	selected->x_rot = node->x_rot;
	selected->y_rot = node->y_rot;
	selected->z_rot = node->z_rot;
	selected->x_scal = node->x_scal;
	selected->y_scal = node->y_scal;
	selected->z_scal = node->z_scal;
	selected->type = node->type;
	selected->material = node->material;
	selected->t = node->t;
	selected->t_inv = node->t_inv;
	selected->tstar = node->tstar;
	selected->tstar_inv = node->tstar_inv;
	if (node->getMeshChecksum()) { selected->mesh->copyMesh(node->mesh);	}
	selected->bound->copyBoundingVolume(node->bound);
}

void SceneGraph::copyNode(Node *node, Node *copy)
{
	// Copy node
	selected = node;
	createNode();
	copyNode(copy);

	Node *temp = selected;

	// Recursively copy all children
	if (copy->child != NULL)
		copyNode(selected, copy->child);

	if (copy->next_sibling != NULL && copy->next_sibling != temp && node != copy->parent)
		copyNode(node, copy->next_sibling);

	selected = temp;

	temp = NULL;
	delete temp;
}

Node* SceneGraph::getRootNode()
{
	return root;
}

Node* SceneGraph::getSelectedNode()
{
	return selected;
}

void SceneGraph::updateMaterial(int material, Node *node)
{
	node->setMaterial(material);

	// Recursively update all children
	if (node->child != NULL)
		updateMaterial(material, node->child);
	
	// Siblings are not affected by each other's material type
	if (node->next_sibling != NULL && node != selected)
		updateMaterial(material, node->next_sibling);
}

void SceneGraph::updateTransformation(Node *node)
{
	double mat[16];
	
	glPushMatrix();
		glTranslated(node->x_trans, node->y_trans, node->z_trans);
		glRotated(node->x_rot, 1, 0, 0);
		glRotated(node->y_rot, 0, 1, 0);
		glRotated(node->z_rot, 0, 0, 1);
		glScaled(node->x_scal, node->y_scal, node->z_scal);

		glGetDoublev(GL_MODELVIEW_MATRIX, mat);

		node->setTransformation(mat); // Updates the node's transformation matrix

		// Recursively update all children
		if (node->child != NULL)
			updateTransformation(node->child);
	glPopMatrix();
	
	// Siblings are not affected by each other's transformations
	if (node->next_sibling != NULL)
		updateTransformation(node->next_sibling);
}

void SceneGraph::updateBoundingVolume(Node *node)
{
	double mat[16];
	
	glPushMatrix();
		glTranslated(node->x_trans, node->y_trans, node->z_trans);
		glRotated(node->x_rot, 1, 0, 0);
		glRotated(node->y_rot, 0, 1, 0);
		glRotated(node->z_rot, 0, 0, 1);
		glScaled(node->x_scal, node->y_scal, node->z_scal);

		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		
		// Recursively update all children
		if (node->child != NULL)
			updateBoundingVolume(node->child);
	glPopMatrix();

	node->computeBoundingVolume(); // Update node bounding box

	// Siblings are not affected by each other's transformations
	if (node->next_sibling != NULL)
		updateBoundingVolume(node->next_sibling);

	glPushMatrix();
		// Calculate local transformation if not root node
		if (node != root) {
			if (node->parent->t.det() != 0) {
				mat4 t_inv = node->parent->t.inverse();		
				mat[0] = t_inv[0][0]; mat[4] = t_inv[0][1]; mat[8] = t_inv[0][2]; mat[12] = t_inv[0][3];
				mat[1] = t_inv[1][0]; mat[5] = t_inv[1][1]; mat[9] = t_inv[1][2]; mat[13] = t_inv[1][3];
				mat[2] = t_inv[2][0]; mat[6] = t_inv[2][1]; mat[10] = t_inv[2][2]; mat[14] = t_inv[2][3];
				mat[3] = t_inv[3][0]; mat[7] = t_inv[3][1]; mat[11] = t_inv[3][2]; mat[15] = t_inv[3][3];
				glMultMatrixd(mat);
			}
		}

		// Reset group bounding box
		node->bound->grp_aabb_min_x = node->bound->grp_aabb_min_y = node->bound->grp_aabb_min_z = INF;
		node->bound->grp_aabb_max_x = node->bound->grp_aabb_max_y = node->bound->grp_aabb_max_z = -INF;
		node->bound->grp_bs_rad = -1; node->bound->grp_bs_x = 0; node->bound->grp_bs_y = 0; node->bound->grp_bs_z = 0;

		updateGroupBoundingVolume(node, node); // Check group bounding box against all children
	glPopMatrix();
}

void SceneGraph::updateGroupBoundingVolume(Node *current, Node *node)
{
	// Compares the current node's bounding values with its children's
	current->computeGroupBoundingVolume(node);

	// Recursively compare min/max values
	if (node->child != NULL)
		updateGroupBoundingVolume(current, node->child);
	if (node->next_sibling != NULL && node != selected)
		updateGroupBoundingVolume(current, node->next_sibling);
}

void SceneGraph::updateBoundingVolumeType()
{
	if (bv_type < NUM_OF_BVTYPE)
		bv_type++;
	else
		bv_type = NONE;

	updateBoundingVolumeType(root);
}

void SceneGraph::updateBoundingVolumeType(Node *node)
{
	node->setBoundType(bv_type);

	// Recursively set bounding volume type for all children
	if (node->child != NULL)
		updateBoundingVolumeType(node->child);

	if (node->next_sibling != NULL)
		updateBoundingVolumeType(node->next_sibling);
}

void SceneGraph::drawScene()
{
	glPushMatrix();
		glLoadIdentity();
		updateTransformation(root); // Update node transformations
		updateBoundingVolume(root); // Update node bounding volumes
	glPopMatrix();

	drawNode(root);
	drawBound(root);
}

void SceneGraph::drawNode(Node *node)
{
	glPushMatrix();
		// Apply transformation
		glTranslated(node->x_trans, node->y_trans, node->z_trans);
		glRotated(node->x_rot, 1, 0, 0);
		glRotated(node->y_rot, 0, 1, 0);
		glRotated(node->z_rot, 0, 0, 1);
		glScaled(node->x_scal, node->y_scal, node->z_scal);

		// Set node material
		double color[4];
		color[0] = materials[node->material]->r;
		color[1] = materials[node->material]->g;
		color[2] = materials[node->material]->b;	
		color[3] = 1.0;		

		// Geometric primitives
		switch (node->type) {
			case NOTHING:	{
							if (node == selected) {
								glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
									glDisable(GL_LIGHTING);
									glLineWidth(1.0);
									glBegin(GL_LINES);
									glColor3f(1.0, 0.0, 0.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(1.0, 0.0, 0.0);
									glColor3f(0.0, 1.0, 0.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(0.0, 1.0, 0.0);
									glColor3f(0.0, 0.0, 1.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(0.0, 0.0, 1.0);
									glColor3f(1.0, 1.0, 1.0);
									glEnd();
									glEnable(GL_LIGHTING);
								glPopAttrib();
							}
							break;
							}
			case SPHERE:	{ glColor4dv(color); glutSolidSphere(1.0f, 20.0f, 20.0f); } break;
			case CUBE:		{ glColor4dv(color); glutSolidCube(1.0f); } break;
			case MESH:		{
							glColor4dv(color);
							for (int ii = 0; ii < node->mesh->triangles.size(); ii++) {
								glBegin(GL_TRIANGLES);
								glNormal3f(node->mesh->triangles[ii]->normal[0],
										   node->mesh->triangles[ii]->normal[1],
										   node->mesh->triangles[ii]->normal[2]);
								glVertex3f(node->mesh->points[node->mesh->triangles[ii]->points[0]][0],
										   node->mesh->points[node->mesh->triangles[ii]->points[0]][1],
										   node->mesh->points[node->mesh->triangles[ii]->points[0]][2]);
								glVertex3f(node->mesh->points[node->mesh->triangles[ii]->points[1]][0],
										   node->mesh->points[node->mesh->triangles[ii]->points[1]][1],
										   node->mesh->points[node->mesh->triangles[ii]->points[1]][2]);
								glVertex3f(node->mesh->points[node->mesh->triangles[ii]->points[2]][0],
										   node->mesh->points[node->mesh->triangles[ii]->points[2]][1],
										   node->mesh->points[node->mesh->triangles[ii]->points[2]][2]);
								glEnd();
							}
							break;
							}
			default:		break;
		}
		
		// Recursively draw all children
		if (node->child != NULL)
			drawNode(node->child);
	glPopMatrix();

	// Siblings are not affected by each other's transformations
	if (node->next_sibling != NULL)
		drawNode(node->next_sibling);
}

void SceneGraph::drawBound(Node *node)
{
	// Bounding box
	if (node == selected) {
		// If not no bounding box
		if (node->bound->bv_type != NONE) {
			drawBoundingVolume(node);
			if (node->child != NULL) {
				drawGroupBoundingVolume(node);
			}
		}
	}
	
	// Recursively update all children
	if (node->child != NULL)
		drawBound(node->child);

	if (node->next_sibling != NULL)
		drawBound(node->next_sibling);
}

void SceneGraph::drawBoundingVolume(Node *node)
{
	if (node->bound->bv_type == AABB) {
		glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
			glDisable(GL_LIGHTING);
			glLineWidth(1.0);
			glColor3f(1,0,0);
			glBegin(GL_LINE_LOOP);
				glVertex3d(node->bound->aabb_max_x, node->bound->aabb_max_y, node->bound->aabb_min_z); // 0
				glVertex3d(node->bound->aabb_min_x, node->bound->aabb_max_y, node->bound->aabb_min_z); // 1
				glVertex3d(node->bound->aabb_min_x, node->bound->aabb_min_y, node->bound->aabb_min_z); // 2
				glVertex3d(node->bound->aabb_max_x, node->bound->aabb_min_y, node->bound->aabb_min_z); // 3
			glEnd();

			glBegin (GL_LINE_LOOP);
				glVertex3d(node->bound->aabb_max_x, node->bound->aabb_min_y, node->bound->aabb_max_z); // 4
				glVertex3d(node->bound->aabb_max_x, node->bound->aabb_max_y, node->bound->aabb_max_z); // 5
				glVertex3d(node->bound->aabb_min_x, node->bound->aabb_max_y, node->bound->aabb_max_z); // 6
				glVertex3d(node->bound->aabb_min_x, node->bound->aabb_min_y, node->bound->aabb_max_z); // 7
			glEnd();

			glBegin (GL_LINE_LOOP);
				glVertex3d(node->bound->aabb_max_x, node->bound->aabb_max_y, node->bound->aabb_min_z); // 0
				glVertex3d(node->bound->aabb_max_x, node->bound->aabb_max_y, node->bound->aabb_max_z); // 5
				glVertex3d(node->bound->aabb_min_x, node->bound->aabb_max_y, node->bound->aabb_max_z); // 6
				glVertex3d(node->bound->aabb_min_x, node->bound->aabb_max_y, node->bound->aabb_min_z); // 1
			glEnd();

			glBegin (GL_LINE_LOOP);
				glVertex3f(node->bound->aabb_max_x, node->bound->aabb_min_y, node->bound->aabb_max_z); // 4
				glVertex3f(node->bound->aabb_min_x, node->bound->aabb_min_y, node->bound->aabb_max_z); // 7
				glVertex3f(node->bound->aabb_min_x, node->bound->aabb_min_y, node->bound->aabb_min_z); // 2
				glVertex3f(node->bound->aabb_max_x, node->bound->aabb_min_y, node->bound->aabb_min_z); // 3
			glEnd();
			glEnable(GL_LIGHTING);
		glPopAttrib();
	}
	else if (node->bound->bv_type == OBB) {
		glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
			glDisable(GL_LIGHTING);
			glLineWidth(1.0);
			glColor3f(1,0,0);
			glPushMatrix();
				double mat[16];
				mat[0] = node->t[0][0]; mat[4] = node->t[0][1]; mat[8] = node->t[0][2]; mat[12] = node->t[0][3];
				mat[1] = node->t[1][0]; mat[5] = node->t[1][1]; mat[9] = node->t[1][2]; mat[13] = node->t[1][3];
				mat[2] = node->t[2][0]; mat[6] = node->t[2][1]; mat[10] = node->t[2][2]; mat[14] = node->t[2][3];
				mat[3] = node->t[3][0]; mat[7] = node->t[3][1]; mat[11] = node->t[3][2]; mat[15] = node->t[3][3];
				glMultMatrixd(mat);
				glBegin(GL_LINE_LOOP);
					glVertex3d(node->bound->obb_max_x, node->bound->obb_max_y, node->bound->obb_min_z); // 0
					glVertex3d(node->bound->obb_min_x, node->bound->obb_max_y, node->bound->obb_min_z); // 1
					glVertex3d(node->bound->obb_min_x, node->bound->obb_min_y, node->bound->obb_min_z); // 2
					glVertex3d(node->bound->obb_max_x, node->bound->obb_min_y, node->bound->obb_min_z); // 3
				glEnd();

				glBegin (GL_LINE_LOOP);
					glVertex3d(node->bound->obb_max_x, node->bound->obb_min_y, node->bound->obb_max_z); // 4
					glVertex3d(node->bound->obb_max_x, node->bound->obb_max_y, node->bound->obb_max_z); // 5
					glVertex3d(node->bound->obb_min_x, node->bound->obb_max_y, node->bound->obb_max_z); // 6
					glVertex3d(node->bound->obb_min_x, node->bound->obb_min_y, node->bound->obb_max_z); // 7
				glEnd();

				glBegin (GL_LINE_LOOP);
					glVertex3d(node->bound->obb_max_x, node->bound->obb_max_y, node->bound->obb_min_z); // 0
					glVertex3d(node->bound->obb_max_x, node->bound->obb_max_y, node->bound->obb_max_z); // 5
					glVertex3d(node->bound->obb_min_x, node->bound->obb_max_y, node->bound->obb_max_z); // 6
					glVertex3d(node->bound->obb_min_x, node->bound->obb_max_y, node->bound->obb_min_z); // 1
				glEnd();

				glBegin (GL_LINE_LOOP);
					glVertex3f(node->bound->obb_max_x, node->bound->obb_min_y, node->bound->obb_max_z); // 4
					glVertex3f(node->bound->obb_min_x, node->bound->obb_min_y, node->bound->obb_max_z); // 7
					glVertex3f(node->bound->obb_min_x, node->bound->obb_min_y, node->bound->obb_min_z); // 2
					glVertex3f(node->bound->obb_max_x, node->bound->obb_min_y, node->bound->obb_min_z); // 3
				glEnd();
			glPopMatrix();
			glEnable(GL_LIGHTING);
		glPopAttrib();
	}
	else if (node->bound->bv_type == BS) {
		glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
			glPushMatrix();
				glDisable(GL_LIGHTING);
				glLineWidth(1.0);
				glColor3f(1,0,0);
				glTranslatef(node->bound->bs_x, node->bound->bs_y, node->bound->bs_z);
				for (double step = 0; step < 360; step += 12) {
					glRotatef(30, 0.0, 1.0, 0.0);
					glBegin(GL_LINE_LOOP);
						for (int ii = 0; ii < 360; ii++) {
							float degInRad = ii*Deg2Rad;
							glVertex2f(cos(degInRad)*node->bound->bs_rad, sin(degInRad)*node->bound->bs_rad);
						} 
					glEnd();
				}
				glRotatef(90, 1.0, 0.0, 0.0);
				glBegin(GL_LINE_LOOP);
					for (int ii = 0; ii < 360; ii++) {
						float degInRad = ii*Deg2Rad;
						glVertex2f(cos(degInRad)*node->bound->bs_rad, sin(degInRad)*node->bound->bs_rad);
					} 
				glEnd();
				glEnable(GL_LIGHTING);
			glPopMatrix();
		glPopAttrib();
	}
}

void SceneGraph::drawGroupBoundingVolume(Node *node)
{	
	if (node->bound->bv_type == AABB || node->bound->bv_type == OBB) {
		glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
			glDisable(GL_LIGHTING);
			glLineStipple(1, 0xAAAA);
			glEnable(GL_LINE_STIPPLE);
			glLineWidth(1.0);
			glColor3f(1,0,0);
			glBegin(GL_LINE_LOOP);
				glVertex3d(node->bound->grp_aabb_max_x, node->bound->grp_aabb_max_y, node->bound->grp_aabb_min_z); // 0
				glVertex3d(node->bound->grp_aabb_min_x, node->bound->grp_aabb_max_y, node->bound->grp_aabb_min_z); // 1
				glVertex3d(node->bound->grp_aabb_min_x, node->bound->grp_aabb_min_y, node->bound->grp_aabb_min_z); // 2
				glVertex3d(node->bound->grp_aabb_max_x, node->bound->grp_aabb_min_y, node->bound->grp_aabb_min_z); // 3
			glEnd();

			glBegin (GL_LINE_LOOP);
				glVertex3d(node->bound->grp_aabb_max_x, node->bound->grp_aabb_min_y, node->bound->grp_aabb_max_z); // 4
				glVertex3d(node->bound->grp_aabb_max_x, node->bound->grp_aabb_max_y, node->bound->grp_aabb_max_z); // 5
				glVertex3d(node->bound->grp_aabb_min_x, node->bound->grp_aabb_max_y, node->bound->grp_aabb_max_z); // 6
				glVertex3d(node->bound->grp_aabb_min_x, node->bound->grp_aabb_min_y, node->bound->grp_aabb_max_z); // 7
			glEnd();

			glBegin (GL_LINE_LOOP);
				glVertex3d(node->bound->grp_aabb_max_x, node->bound->grp_aabb_max_y, node->bound->grp_aabb_min_z); // 0
				glVertex3d(node->bound->grp_aabb_max_x, node->bound->grp_aabb_max_y, node->bound->grp_aabb_max_z); // 5
				glVertex3d(node->bound->grp_aabb_min_x, node->bound->grp_aabb_max_y, node->bound->grp_aabb_max_z); // 6
				glVertex3d(node->bound->grp_aabb_min_x, node->bound->grp_aabb_max_y, node->bound->grp_aabb_min_z); // 1
			glEnd();

			glBegin (GL_LINE_LOOP);
				glVertex3f(node->bound->grp_aabb_max_x, node->bound->grp_aabb_min_y, node->bound->grp_aabb_max_z); // 4
				glVertex3f(node->bound->grp_aabb_min_x, node->bound->grp_aabb_min_y, node->bound->grp_aabb_max_z); // 7
				glVertex3f(node->bound->grp_aabb_min_x, node->bound->grp_aabb_min_y, node->bound->grp_aabb_min_z); // 2
				glVertex3f(node->bound->grp_aabb_max_x, node->bound->grp_aabb_min_y, node->bound->grp_aabb_min_z); // 3
			glEnd();
			glEnable(GL_LIGHTING);
		glPopAttrib();
	}
	else if (node->bound->bv_type == BS) {
		glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
			glPushMatrix();
				glDisable(GL_LIGHTING);
				glLineStipple(1, 0xAAAA);
				glEnable(GL_LINE_STIPPLE);
				glLineWidth(1.0);
				glColor3f(1,0,0);
				glTranslatef(node->bound->grp_bs_x, node->bound->grp_bs_y, node->bound->grp_bs_z);
				for (double step = 0; step < 360; step += 180) {
					glRotatef(90, 0.0, 1.0, 0.0);
					glBegin(GL_LINE_LOOP);
						for (int ii = 0; ii < 360; ii++) {
							float degInRad = ii*Deg2Rad;
							glVertex2f(cos(degInRad)*node->bound->grp_bs_rad, sin(degInRad)*node->bound->grp_bs_rad);
						} 
					glEnd();
				}
				glRotatef(90, 1.0, 0.0, 0.0);
				glBegin(GL_LINE_LOOP);
					for (int ii = 0; ii < 360; ii++) {
						float degInRad = ii*Deg2Rad;
						glVertex2f(cos(degInRad)*node->bound->grp_bs_rad, sin(degInRad)*node->bound->grp_bs_rad);
					} 
				glEnd();
				glEnable(GL_LIGHTING);
			glPopMatrix();
		glPopAttrib();
	}
}

void SceneGraph::createTable()
{
	createNode(); // Table root node

	createNode(); // Place marker
	selected->setTranslation(0.0, 4.2, 0.0);
	selected->setType(NOTHING);
	selected = selected->parent;

	createNode(); // Table top
	selected->setTranslation(0.0, 4.1, 0.0);
	selected->setScaling(4.0, 0.2, 6.0);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Table leg
	selected->setTranslation(1.9, 2.0, 2.9);
	selected->setScaling(0.2, 4.0, 0.2);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Table leg
	selected->setTranslation(1.9, 2.0, -2.9);
	selected->setScaling(0.2, 4.0, 0.2);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Table leg
	selected->setTranslation(-1.9, 2.0, 2.9);
	selected->setScaling(0.2, 4.0, 0.2);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Table leg
	selected->setTranslation(-1.9, 2.0, -2.9);
	selected->setScaling(0.2, 4.0, 0.2);
	selected->setType(CUBE);
	selected = selected->parent;
	
	selected = selected->child; // Go back to place marker node
}

void SceneGraph::createChair()
{
	createNode(); // Chair root node

	createNode(); // Place marker
	selected->setTranslation(0.0, 3.05, 0.0);
	selected->setType(NOTHING);
	selected = selected->parent;

	createNode(); // Chair seat
	selected->setTranslation(0.0, 2.95, 0.0);
	selected->setScaling(2.0, 0.2, 2.0);
	selected->setType(CUBE);
	selected = selected->parent;

	createNode(); // Chair back
	selected->setTranslation(-0.9, 4.7, 0.0);
	selected->setScaling(0.2, 3.4, 2.0);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Chair leg
	selected->setTranslation(0.9, 1.45, 0.9);
	selected->setScaling(0.2, 2.9, 0.2);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Chair leg
	selected->setTranslation(0.9, 1.45, -0.9);
	selected->setScaling(0.2, 2.9, 0.2);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Chair leg
	selected->setTranslation(-0.9, 1.45, 0.9);
	selected->setScaling(0.2, 2.9, 0.2);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Chair leg
	selected->setTranslation(-0.9, 1.45, -0.9);
	selected->setScaling(0.2, 2.9, 0.2);
	selected->setType(CUBE);
	selected = selected->parent;
	
	selected = selected->child; // Go back to place marker node
}

void SceneGraph::createCabinet()
{
	createNode(); // Cabinet root node

	createNode(); // Place marker
	selected->setTranslation(0.0, 6.0, 0.0);
	selected->setType(NOTHING);
	selected = selected->parent;

	createNode(); // Cabinet body
	selected->setTranslation(0.0, 3.0, 0.0);
	selected->setScaling(1.5, 6.0, 3.0);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Cabinet drawer top
	selected->setTranslation(0.9, 4.9, 0.0);
	selected->setScaling(0.3, 1.4, 2.4);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Cabinet drawer middle
	selected->setTranslation(0.9, 3.0, 0.0);
	selected->setScaling(0.3, 1.4, 2.4);
	selected->setType(CUBE);
	selected = selected->parent;
	
	createNode(); // Cabinet drawer bottom
	selected->setTranslation(0.9, 1.1, 0.0);
	selected->setScaling(0.3, 1.4, 2.4);
	selected->setType(CUBE);
	selected = selected->parent;
	
	selected = selected->child; // Go back to place marker node
}

string SceneGraph::createMesh(string filename)
{
	createNode(); // Mesh root node

	createNode(); // Place marker
	selected->setType(NOTHING);
	selected = selected->parent;

	createNode();
	selected->setType(MESH);
	selected->mesh = new Mesh(filename);
	// If failed to create mesh
	if (!selected->mesh->getChecksum()) {
		string status = selected->mesh->getStatusMsg();
		deleteNode();
		deleteNode();
		return status;
	}
	else {
		string status = selected->mesh->getStatusMsg();
		double mesh_height = selected->mesh->getHeight();
		selected = selected->parent;
		selected = selected->child; // Go back to place marker node		
		selected->setTranslation(selected->getTranslation()[0], selected->getTranslation()[1] + mesh_height, selected->getTranslation()[2]);
		return status;
	}
}

void SceneGraph::renderScene(vec3 eyep, vec3 vdir, vec3 uvec, double fovy, bool setting[])
{
	// Cache the transformation matrices needed for the intersection tests
	cacheScene(root);

	// Raytrace
	Raytrace *raytracer = new Raytrace(width, height, materials, root, setting);

	raytracer->initLighting(lpos, lcol, acol); // Set lights
	raytracer->initCamera(eyep, vdir, uvec, fovy); // Set camera
	raytracer->render(); // Raytrace scene

	delete raytracer;
}

void SceneGraph::cacheScene(Node *node)
{
	node->computeTransformations(); // Generates the matrices needed for raytracing
	glPushMatrix();
		glLoadIdentity();
		node->bound->computeBVHMatrix(); // Generate bounding volumes for BVH
	glPopMatrix();

	// Recursively draw all children
	if (node->child != NULL)
		cacheScene(node->child);
	
	// Siblings are not affected by each other's transformations
	if (node->next_sibling != NULL)
		cacheScene(node->next_sibling);
}