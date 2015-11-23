#include "Raytrace.h"

Raytrace::Raytrace()
{
}

Raytrace::Raytrace(double width, double height, vector<Material*> mat, Node* rt, bool setting[])
{
	// Initialize parameters
	reso_x = width;
	reso_y = height;
	materials = mat;
	root = rt;
	render_setting = setting;
	progress_flag = false;
}

Raytrace::~Raytrace()
{
	materials.clear();
}

void Raytrace::initLighting(vec3 lp, vec3 lc, vec3 ac)
{
	lpos = lp; // Point light
	lcol = lc; // Point light color
	acol = ac; // Ambient light color
}

void Raytrace::initCamera(vec3 ep, vec3 vd, vec3 uv, double fy)
{
	eyep = ep; // Eye position
	vdir = vd; // Viewing direction
	uvec = uv; // World space up vector
	fovy = fy; // Total field of view in Y
	
	vec3 a = vdir^uvec; // Be sure this is well defined
	vec3 b = vdir^a; // B is now in the correct plane
	midpoint = eyep + vdir; // Now have midpoint of screen
	h_px = (double)(reso_x/reso_y)*vdir.length()*tan((fovy*0.5)*Deg2Rad)*a.normalized(); // Rescale A to H
	v_px = vdir.length()*tan((fovy*0.5)*Deg2Rad)*b.normalized(); // Rescale B to V
}

Ray* Raytrace::initRay(vec3 o, vec3 dir)
{
	Ray *ray = new Ray();
	ray->o = o;
	ray->d = dir;
	return ray;
}

void Raytrace::render()
{
	int size = 3*reso_x*reso_y;
	unsigned char *output = new unsigned char[size]; // Output file
	char *outfile = new char();	outfile = "output.bmp"; // Output filename
	progress_flag = true; progress_bar = 0; // Progress bar

	// Generate rays for every pixel on screen
	for (int y = 0; y < reso_y; y++) {
		for (int x = 0; x < reso_x; x++) {
			// If supersampling is on
			if (render_setting[1]) {
				vec3 acc(0.0, 0.0, 0.0);
				for (double dx = -1; dx < 2; dx++ ){
					for (double dy = -1; dy < 2; dy++ ){
						// Any point in NDC is (Sx, Sy)
						vec3 orig = midpoint + (2.0*(((double)x + dx*0.5)/(reso_x - 1)) - 1)*h_px + (2.0*(((double)y + dy*0.5)/(reso_y - 1)) - 1)*v_px;
						vec3 dir = (orig - eyep).normalize();
						Ray *ray = initRay(eyep, dir);
						vec3 color(0.0, 0.0, 0.0);

						// Bounce, my little rays, BOUNCE!
						traceRay(ray, 0, color);
						acc += color;

						delete ray;
					}
				}
				// Stores the color
				int index = (reso_x*y + x)*3;
				output[index + 0] = acc[0]*255.0/9;
				output[index + 1] = acc[1]*255.0/9;
				output[index + 2] = acc[2]*255.0/9;
			}
			// Render without supersampling
			else {
				// Any point in NDC is (Sx, Sy)
				vec3 o = midpoint + (2.0*(double)(x/(reso_x - 1)) - 1)*h_px + (2.0*(double)(y/(reso_y - 1)) - 1)*v_px;
				vec3 dir = (o - eyep).normalize();
				Ray *ray = initRay(eyep, dir);
				vec3 color(0.0, 0.0, 0.0);

				// Bounce my little rays, BOUNCE!
				traceRay(ray, 0, color);

				delete ray;

				// Stores the color
				int index = (reso_x*y + x)*3;
				output[index + 0] = color[0]*255.0;
				output[index + 1] = color[1]*255.0;
				output[index + 2] = color[2]*255.0;
			}
		}
		if (y%10 == 0) { progress_bar = (y/reso_y)*100.0; }
		if (progress_flag) renderProgress();
	}
	progress_bar = 100;
	for (int ii = 0; ii < 100; ii++ ) { renderProgress(); }

	// Saves the data to a BMP image
	SOIL_save_image(outfile, SOIL_SAVE_TYPE_BMP, reso_x, reso_y, 3, output);

	progress_flag = false; progress_bar = 0;
	delete output;
}

void Raytrace::traceRay(Ray *ray, int depth, vec3 &color)
{
	vec3 bcol(0.0, 0.0, 0.0); // Background color

	double t = -1.0; // Intersection
	vec3 normal(0.0, 0.0, 0.0); // Normal at intersection
	Node* node = NULL; // Intersecting node
	
	if (depth > max_depth) {
		color = bcol;
		return;
	}

	// Check for intersection using Hierarchical Bounding Volumes
	hierarchicalBoundingVolumes(ray, root, t, normal, node);

	// If there was no intersection
	if (t < EPSILON) {
		// Check if ray is parallel to point light
		vec3 ldir(lpos - eyep);
		if (acos(ldir*ray->d.normalized()) < EPSILON) {
			color = lcol;
			return;
		}
		else {
			color = bcol;
			return;
		}
	}
	else {
		vec3 intersection = ray->o + t*ray->d; // Get the point of intersection
		ray->dist = t;

		// Material color
		vec3 objcol(materials[node->getMaterial()]->r, materials[node->getMaterial()]->g, materials[node->getMaterial()]->b);
		// Diffuse and ambient
		computeDiffuseAmbient(intersection, normal, ray, materials[node->getMaterial()], node, depth, objcol, color);
		// Transparency
		//computeTransparency(intersection, normal, ray, materials[node->getMaterial()], node, depth, objcol, color);
		// Reflection
		computeReflection(intersection, normal, ray, materials[node->getMaterial()], node, depth, objcol, color);
		// Refraction
		if (!render_setting[0])
			computeRefraction(intersection, normal, ray, materials[node->getMaterial()], node, depth, objcol, color);
		// Fresnel Reflection
		if (render_setting[0])
			computeFresnelReflection(intersection, normal, ray, materials[node->getMaterial()], node, depth, objcol, color);
		// Blinn-Phong specular
		computeSpecular(intersection, normal, ray, materials[node->getMaterial()], node, depth, objcol, color);
	}
}

void Raytrace::computeDiffuseAmbient(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color)
{
	if (materials[node->getMaterial()]->transparency == 0) {
		color += prod(acol, objcol); // Ambient color
	
		// Check if any color goes over 1 (255)
		if(color[0] > 1) color[0] = 1;
		if(color[1] > 1) color[1] = 1;
		if(color[2] > 1) color[2] = 1;
	}
}

void Raytrace::computeTransparency(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color)
{
	if (materials[node->getMaterial()]->transparency > 0) {
		Ray *transray = initRay(intersection, ray->d);
		traceRay(transray, depth + 1, color);
		
		// Beer's law
		vec3 transparency(1.0, 1.0, 1.0);
		if (insideObject(normal, ray)) {
			vec3 absorbance = objcol*0.15*-ray->dist;
			for (int ii = 0; ii < 3; ii++) {
				if (absorbance[ii] != 0) transparency[ii] = expf(absorbance[ii]);
				else transparency[ii] = 0;
			}
		}

		color = prod(color, transparency); // Transparent color

		// Check if any color goes over 1 (255)
		if(color[0] > 1) color[0] = 1;
		if(color[1] > 1) color[1] = 1;
		if(color[2] > 1) color[2] = 1;
	}
}

void Raytrace::computeSpecular(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color)
{
	if (shadowRayUnblocked(intersection, lpos)) {
		if (material->transparency == 0) {
			vec3 lvec = (lpos - intersection).normalized(); // Light vector

			// Get diffuse term
			double ndotl = normal*lvec;
			vec3 diffuse = prod(objcol, lcol)*max(EPSILON, ndotl);
		
			// Get specular term (using reflect instead of half vector)
			// Vector3 half = (L + Lvec).normalisedCopy();
	 		// double HdotN = max(0.0f, N.dotProduct(half));
			vec3 refl = (-lvec - (2.0f*(-lvec*normal))*normal).normalized();
			double rdotv = max(EPSILON, refl*(eyep - intersection).normalized());
			vec3 specular = lcol*powf(rdotv, materials[node->getMaterial()]->specular);

			color += diffuse + specular; // Add diffuse and specular
		}
	}
	
	// Check if any color goes over 1 (255)
	if(color[0] > 1) color[0] = 1;
	if(color[1] > 1) color[1] = 1;
	if(color[2] > 1) color[2] = 1;
}

void Raytrace::computeReflection(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color)
{
	if (materials[node->getMaterial()]->reflectivity > 0 && materials[node->getMaterial()]->transparency == 0) {
		// Compute the reflected ray
		double cosI = -(ray->d.normalized()*normal);
		vec3 refldir = ray->d.normalized() + 2*prod(normal, cosI);

		// Compute reflected ray
		Ray *reflray = initRay(intersection, refldir);
		vec3 reflcol(0, 0, 0);
		traceRay(reflray, depth + 1, reflcol);

		color = reflcol*material->reflectivity + color*(1 - material->reflectivity);
	
		// Check if any color goes over 1 (255)
		if(color[0] > 1) color[0] = 1;
		if(color[1] > 1) color[1] = 1;
		if(color[2] > 1) color[2] = 1;
	}
}

void Raytrace::computeRefraction(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color)
{
	if (materials[node->getMaterial()]->transparency > 0) {
		// Compute the refraction index
		double n;
		double n1 = 1.0;
		double n2 = material->refraction;
		// Check if ray is inside the object
		if (insideObject(normal, ray))
			swap(n1, n2);
		n = n1/n2;

		double cosI = -(ray->d.normalized()*normal);
		double sinT2 = n*n*(1.0 - cosI*cosI);
		if (sinT2 < 1.0) {
			vec3 refrdir = n*ray->d - (n*cosI - sqrt(1.0 - sinT2))*normal;

			// Compute refracted ray
			Ray *refrray = initRay(intersection, refrdir);
			vec3 refrcol(0, 0, 0);
			traceRay(refrray, depth + 1, refrcol);
		
			// Beer's law
			vec3 transparency(1.0, 1.0, 1.0);
			if (insideObject(normal, ray)) {
				vec3 absorbance = objcol*0.15*-ray->dist;
				for (int ii = 0; ii < 3; ii++) {
					if (absorbance[ii] != 0) transparency[ii] = expf(absorbance[ii]);
					else transparency[ii] = 0;
				}
			}

			color = prod(refrcol, transparency);
		}

		// Check if any color goes over 1 (255)
		if(color[0] > 1) color[0] = 1;
		if(color[1] > 1) color[1] = 1;
		if(color[2] > 1) color[2] = 1;
	}
}

void Raytrace::computeFresnelReflection(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color)
{
	if (materials[node->getMaterial()]->transparency > 0) {
		vec3 refldir, refrdir;
		vec3 reflcol(0, 0, 0); vec3 refrcol(0, 0, 0);
		vec3 transparency(1.0, 1.0, 1.0);
		double reflectance, transmittance;
		reflectance = 1.0;
		transmittance = 1.0 - reflectance;

		// Compute the refraction index
		double n;
		double n1 = 1.0;
		double n2 = material->refraction;
		// Check if ray is inside the object
		if (insideObject(normal, ray))
			swap(n1, n2);
		n = n1/n2;

		double cosI = (ray->d.normalized()*normal);
		double sinT2 = n*n*(1.0 - cosI*cosI);
		if (sinT2 < 1.0) {
			//double cosT = sqrt(1.0 - sinT2);
			//double r0rth = (n1*cosI - n2*cosT)/(n1*cosI + n2*cosT);
			//double rPar = (n2*cosI - n1*cosT)/(n2*cosI + n1*cosT);

			//reflectance = (r0rth*r0rth + rPar*rPar)/2.0;
			//transmittance = 1.0 - reflectance;

			// Refraction
			refrdir = n*ray->d - (n*cosI - sqrt(1.0 - sinT2))*normal;
			Ray *refrray = initRay(intersection, refrdir);
			traceRay(refrray, depth + 1, refrcol);
		
			// Beer's law
			if (insideObject(normal, ray)) {
				vec3 absorbance = objcol*0.15*-ray->dist;
				for (int ii = 0; ii < 3; ii++) {
					if (absorbance[ii] != 0) transparency[ii] = expf(absorbance[ii]);
					else transparency[ii] = 0;
				}
			}
		}

		// Reflection
		refldir = ray->d.normalized() + 2*prod(normal, cosI);	
		Ray *reflray = initRay(intersection, refldir);
		traceRay(reflray, depth + 1, reflcol);

		reflectance = rSchlick2(intersection, normal, ray, material, node, depth, objcol, color);
		transmittance = 1.0 - reflectance;

		//cout << reflectance << " " << transmittance <<endl;
		
		color = reflcol*reflectance + prod(refrcol, transparency)*transmittance;

		// Check if any color goes over 1 (255)
		if(color[0] > 1) color[0] = 1;
		if(color[1] > 1) color[1] = 1;
		if(color[2] > 1) color[2] = 1;
	}
}

double Raytrace::rSchlick2(vec3 intersection, vec3 normal, Ray *ray, Material *material, Node* node, int depth, vec3 objcol, vec3 &color)
{
	double n1 = 1.0;
	double n2 = material->refraction;
	double r0 = (n1 - n2)/(n1 + n2);
	r0 *= r0;
	double cosX = -(normal*ray->d.normalized());
	if (n1 > n2) {
		double n = n1/n2;
		double sinT2 = n*n*(1.0 - cosX*cosX);
		if (sinT2 > 1.0) return 1.0;
		cosX = sqrt(1.0 - sinT2);
	}
	double x = 1.0 - cosX;
	return r0 + (1.0 - r0)*x*x*x*x*x;
}

bool Raytrace::shadowRayUnblocked(vec3 intersection, vec3 lpos)
{
	vec3 n(0, 0, 0);
	vec3 d(lpos - intersection); // Create a ray pointing from the point of intersection to the light
	vec3 o = intersection + 0.0001*d;
	double t = -1;
	vec3 normal(0.0, 0.0, 0.0);
	Node* node;

		// Shoot the ray with intersection as origin, check if there are any intersections
	Ray *ray = initRay(o, d);
	hierarchicalBoundingVolumes(ray, root, t, normal, node);
	if (t > EPSILON)
		return false;
	return true;
}

bool Raytrace::insideObject(vec3 normal, Ray *ray)
{
	double dir = ray->d.normalized()*normal;

	if (dir > 0) return true;
	return false;
}

void Raytrace::hierarchicalBoundingVolumes(Ray *ray, Node* node, double &tmin, vec3 &nmin, Node* &selected)
{
	double t = -1.0;
	vec3 normal(0.0, 0.0, 0.0);

	double bvh = -1.0;
	bvh = RaySphereAlgebraicIntersection(ray->o, ray->d, node->bound->bvh_t_inv, node->bound->bvh_tstar_inv, node->bound->bvh_t_trans_inv, vec3(0.0, 0.0, 0.0));
	if (bvh > EPSILON) {
		t = rayIntersect(ray, node, normal);
		if (t > EPSILON && (tmin < 0 || t < tmin)) {
			tmin = t;
			nmin = normal;
			selected = node;
		}
	}
	
	// Recursively draw all children
	if (node->child != NULL)
		hierarchicalBoundingVolumes(ray, node->child, tmin, nmin, selected);
	
	// Siblings are not affected by each other's transformations
	if (node->next_sibling != NULL)
		hierarchicalBoundingVolumes(ray, node->next_sibling, tmin, nmin, selected);
}

double Raytrace::rayIntersect(Ray *ray, Node* node, vec3 &nmin)
{
	double t = -1.0;
	double tmin = -1.0;
	vec3 normal(0.0, 0.0, 0.0);

	switch (node->getType()) {
		case SPHERE:	{
						t = RaySphereAlgebraicIntersection(ray->o, ray->d, node->t_inv, node->tstar_inv, node->t_trans_inv, normal);
						if (t > EPSILON && (tmin < 0 || t < tmin)) {
							tmin = t;
							nmin = normal;
						}
						break;
						}
		case CUBE:		{
						t = RayCubeSlabsIntersection(ray->o, ray->d, node->t_inv, node->tstar_inv, node->t_trans_inv, normal);
						if (t > EPSILON && (tmin < 0 || t < tmin)) {
							tmin = t;
							nmin = normal;
						}
						break;
						}
		case MESH:		{
						for (int ii = 0; ii < node->mesh->triangles.size(); ii++) {
							vec3 p1(node->mesh->points[node->mesh->triangles[ii]->points[0]][0],
									node->mesh->points[node->mesh->triangles[ii]->points[0]][1],
									node->mesh->points[node->mesh->triangles[ii]->points[0]][2]);
							vec3 p2(node->mesh->points[node->mesh->triangles[ii]->points[1]][0],
									node->mesh->points[node->mesh->triangles[ii]->points[1]][1],
									node->mesh->points[node->mesh->triangles[ii]->points[1]][2]);
							vec3 p3(node->mesh->points[node->mesh->triangles[ii]->points[2]][0],
									node->mesh->points[node->mesh->triangles[ii]->points[2]][1],
									node->mesh->points[node->mesh->triangles[ii]->points[2]][2]);
							t = RayTriangleMollerTrumboreIntersection(ray->o, ray->d, p1, p2, p3, node->t_inv, node->tstar_inv, node->t_trans_inv, normal);
							if (t > EPSILON && (tmin < 0 || t < tmin)) {
								tmin = t;
								nmin = normal;
							}
						}
						break;
						}
		default: break;
	}

	return tmin;
}

void Raytrace::renderProgress()
{
	// Clearing the depth buffer 
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 640, 0, 480);	
		glScalef(1, -1, 1); // Invert the y axis, down is positive
		glTranslatef(0, -480, 0); // Move the origin from the bottom left corner to the upper left corner
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
			glLoadIdentity();
			glDisable(GL_LIGHTING);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			char progress_txt[1024];

			glRasterPos2f(480, 450);
			sprintf(progress_txt, "Raytracing...%d%%", progress_bar);
			for (unsigned int ii = 0; ii < strlen(progress_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, progress_txt[ii]);
			
			glColor4f(0.9,0.9,0.9,1.0);
			glBegin(GL_QUADS); 
				glVertex2i(480, 463); 
				glVertex2i(480, 467); 
				glVertex2i(480 + (int)(progress_bar*0.01*135), 467); 
				glVertex2i(480 + (int)(progress_bar*0.01*135), 463);
			glEnd();
			
			glEnable(GL_LIGHTING);
			glColor4f(0.3, 0.3, 0.3, 0.2);
			glRectf(470.0, 435.0, 630.0, 475.0);
		glPopMatrix();
		
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();
}