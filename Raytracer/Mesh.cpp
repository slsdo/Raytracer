#include "Mesh.h"

Mesh::Mesh()
{
	checksum = false;
}

Mesh::Mesh(string filename)
{
	checksum = false;
	status_msg = "";

	if (filename == "random" || filename == "random extrude" || filename == "random surfrev") {	
		checksum = true;
		status_msg = "Success!";
		randomMesh(filename);
		generateMesh();
	}
	else if (fileParser(filename)) {		
		checksum = true;
		status_msg = "Success!";
		generateMesh();
	}
}

Mesh::~Mesh()
{
}

bool Mesh::fileParser(string filename)
{
	// Error checking for wrong file types
	if (filename.size() < 9	|| (filename.substr(filename.size()-8, 8) != ".extrude"	&& filename.substr(filename.size()-8, 8) != ".surfrev")) {
		status_msg = "File type incorrect.";
		return false;
	}
	ifstream in(filename.c_str());
	if (in.fail()) { status_msg = "Unable to open the file."; return false; }
	string str;
	getline(in, str); // Get first line

	if (str == "extrusion") {
		mesh_type = EXTRUSION;
		getline(in, str); // Get extrusion length
		if (!atof(str.c_str())) { status_msg = "Invalid extrusion length."; return false; }
		extrude_length = atof(str.c_str()); // Convert to double
		radial_slice = 0;
		getline(in, str); // Get number of points
		if (!atoi(str.c_str())) { status_msg = "Invalid points."; return false; }
		num_of_point = atoi(str.c_str()); // Convert to int
		if (num_of_point < 1) { status_msg = "Not enough points."; return false; }
		points.resize(num_of_point*2);

		// Continue for the number of points given
		for (int ii = 0; ii < num_of_point; ii++) {
			double vert_x, vert_z; vert_x = vert_z = 0.0;
			getline(in, str);
			const char *strc = new char[1024]; strc = str.c_str();
			sscanf(strc, "%lf %lf", &vert_x, &vert_z);
			points[ii] = vec3(vert_x, 0.0, vert_z);
			points[ii + num_of_point] = vec3(vert_x, extrude_length, vert_z);
		}
	}
	else if (str == "surfrev") {
		mesh_type = SURFREV;
		extrude_length = 0.0;
		getline(in, str); // Get radial slices number
		if (!atof(str.c_str())) { status_msg = "Invalid radial slices."; return false; }
		radial_slice = atoi(str.c_str()); // Convert to int
		if (radial_slice < 3) { status_msg = "Insufficient radial slices."; return false; }
		getline(in, str); // Get number of points
		if (!atoi(str.c_str())) { status_msg = "Invalid points."; return false; }
		num_of_point = atoi(str.c_str()); // Convert to int
		if (num_of_point < 1) { status_msg = "Not enough points."; return false; }
		points.resize(num_of_point*radial_slice);
		
		double step = 360.0/radial_slice;
		// Continue for the number of points given
		for (int ii = 0; ii < num_of_point; ii++) {
			double angle = 0.0;
			double vert_x, vert_y; vert_x = vert_y = 0.0;
			getline(in, str);
			const char *strc = new char[1024]; strc = str.c_str();
			sscanf(strc, "%lf %lf", &vert_x, &vert_y);
			if (vert_x < 0) { status_msg = "X cannot be negative."; return false; }
			vec3 pt(vert_x, vert_y, 0.0);
			points[ii] = pt;
			for (int jj = 1; jj < radial_slice; jj++) {
				angle += step;
				if ((angle == 90.0) || (angle == 270.0)) pt[0] = 0.0;// C++ cos(90) and cos(270) floating point error check
				else pt[0] = points[ii][0]*cos(angle*Deg2Rad);
				if ((angle == 0.0) || (angle == 180.0)) pt[2] = 0.0; // C++ sin(0) and cos(180) floating point error check
				else pt[2] = points[ii][0]*sin(angle*Deg2Rad);
				points[ii + jj*num_of_point] = pt;
			}
		}
	}
	height = 0.0;
	for (int ii = 0; ii < points.size(); ii++) {
		if (height < points[ii][1])
			height = points[ii][1];
	}
	in.close();
	return true;
}

void Mesh::randomMesh(string mtype)
{
	srand(time(NULL));

	int mrand = rand()%2;

	if (mtype == "random extrude" || (mtype == "random" && mrand == 0)) {
		mesh_type = EXTRUSION;
		extrude_length = rand()%10 + 1;
		radial_slice = 0;
		num_of_point = rand()%20 + 1;
		points.resize(num_of_point*2);
		int endcap = rand()%2; // Endcap or not
		if (endcap) {
			double vert_x = 6.0*(double)rand()/RAND_MAX - 3;
			double vert_z = 6.0*(double)rand()/RAND_MAX - 3;
			points[0] = vec3(vert_x, 0.0, vert_z);
			points[0 + num_of_point] = vec3(vert_x, extrude_length, vert_z);
			points[num_of_point - 1] = vec3(vert_x, 0.0, vert_z);
			points[num_of_point - 1 + num_of_point] = vec3(vert_x, extrude_length, vert_z);
			for (int ii = 1; ii < num_of_point - 1; ii++) {
				double vert_x = 6.0*(double)rand()/RAND_MAX - 3;
				double vert_z = 6.0*(double)rand()/RAND_MAX - 3;
				points[ii] = vec3(vert_x, 0.0, vert_z);
				points[ii + num_of_point] = vec3(vert_x, extrude_length, vert_z);
			}
		}
		else {
			for (int ii = 0; ii < num_of_point; ii++) {
				double vert_x = 6.0*(double)rand()/RAND_MAX - 3;
				double vert_z = 6.0*(double)rand()/RAND_MAX - 3;
				points[ii] = vec3(vert_x, 0.0, vert_z);
				points[ii + num_of_point] = vec3(vert_x, extrude_length, vert_z);
			}
		}
	}
	else if (mtype == "random surfrev" || (mtype == "random" && mrand == 1)) {
		mesh_type = SURFREV;
		extrude_length = 0.0;
		radial_slice = rand()%40 + 3;
		num_of_point = rand()%20 + 1;
		points.resize(num_of_point*radial_slice);		
		double step = 360.0/radial_slice;
		// Continue for the number of points given
		for (int ii = 0; ii < num_of_point; ii++) {
			double angle = 0.0;
			double vert_x = 4.0*(double)rand()/(double)RAND_MAX + 0.5;
			double vert_y = 10.0*(double)rand()/(double)RAND_MAX - 5;
			vec3 pt(vert_x, vert_y, 0.0);
			points[ii] = pt;
			for (int jj = 1; jj < radial_slice; jj++) {
				angle += step;
				if ((angle == 90.0) || (angle == 270.0)) pt[0] = 0.0;// C++ cos(90) and cos(270) floating point error check
				else pt[0] = points[ii][0]*cos(angle*Deg2Rad);
				if ((angle == 0.0) || (angle == 180.0)) pt[2] = 0.0; // C++ sin(0) and cos(180) floating point error check
				else pt[2] = points[ii][0]*sin(angle*Deg2Rad);
				points[ii + jj*num_of_point] = pt;
			}
		}
	}
	height = 0.0;
	for (int ii = 0; ii < points.size(); ii++) {
		if (height < points[ii][1])
			height = points[ii][1];
	}
}

void Mesh::generateMesh()
{
	if (mesh_type == EXTRUSION) {
		if (isConvex()) {
			for (int ii = 1; ii < num_of_point - 1; ii++) {
				// Bottom endcap
				Triangle *tri_base = new Triangle();
				tri_base->points[0] = 0;
				tri_base->points[1] = ii;
				tri_base->points[2] = ii + 1;
				tri_base->normal = (points[tri_base->points[1]] - points[tri_base->points[0]])^
								   (points[tri_base->points[2]] - points[tri_base->points[1]]).normalize();
				triangles.push_back(tri_base);

				// Top endcap
				Triangle *tri_top = new Triangle();
				tri_top->points[0] = num_of_point;
				tri_top->points[1] = num_of_point + ii + 1;
				tri_top->points[2] = num_of_point + ii;
				tri_top->normal = (points[tri_top->points[1]] - points[tri_top->points[0]])^
								  (points[tri_top->points[2]] - points[tri_top->points[1]]).normalize();
				triangles.push_back(tri_top);
			}
		}
		for(int ii = 0; ii < num_of_point; ii++) {
			// Sides
			Triangle *tri_side0 = new Triangle();
			tri_side0->points[0] = ii;
			tri_side0->points[1] = ii + num_of_point;
			tri_side0->points[2] = (ii + 1)%num_of_point + num_of_point;
			tri_side0->normal = (points[tri_side0->points[1]] - points[tri_side0->points[0]])^
								(points[tri_side0->points[2]] - points[tri_side0->points[1]]).normalize();
			triangles.push_back(tri_side0);
			Triangle *tri_side1 = new Triangle();
			tri_side1->points[0] = ii;
			tri_side1->points[1] = (ii + 1)%num_of_point + num_of_point;
			tri_side1->points[2] = (ii + 1)%num_of_point;
			tri_side1->normal = (points[tri_side0->points[1]] - points[tri_side0->points[0]])^
								(points[tri_side0->points[2]] - points[tri_side0->points[1]]).normalize();
			triangles.push_back(tri_side1);
		}
	}
	else if (mesh_type == SURFREV) {
		if (points[points.size() - 1][0] != 0.0) {
			for(int ii = num_of_point - 1 + num_of_point*2; ii < points.size(); ii += num_of_point) {
				// Top endcap
				Triangle *tri_top = new Triangle();
				tri_top->points[0] = num_of_point - 1;
				tri_top->points[1] = ii;
				tri_top->points[2] = ii - num_of_point;
				tri_top->normal = (points[tri_top->points[1]] - points[tri_top->points[0]])^
								  (points[tri_top->points[2]] - points[tri_top->points[1]]).normalize();
				triangles.push_back(tri_top);
			}
		}
		if (points[0][0] != 0.0) {
			for (int ii = num_of_point*2; ii < points.size(); ii += num_of_point) {
				// Bottom endcap
				Triangle *tri_base = new Triangle();
				tri_base->points[0] = 0;
				tri_base->points[1] = ii - num_of_point;
				tri_base->points[2] = ii;
				tri_base->normal = (points[tri_base->points[1]] - points[tri_base->points[0]])^
								   (points[tri_base->points[2]] - points[tri_base->points[1]]).normalize();
				triangles.push_back(tri_base);
			}
		}
		for(int ii = 0; ii < radial_slice; ii++) {
			for(int jj = 0; jj < num_of_point - 1; jj++) {
				// Sides
				Triangle *tri_side0 = new Triangle();
				tri_side0->points[0] = jj + ii*num_of_point;
				tri_side0->points[1] = jj + ((ii + 1)%radial_slice)*num_of_point + 1;
				tri_side0->points[2] = jj + ((ii + 1)%radial_slice)*num_of_point;
				tri_side0->normal = (points[tri_side0->points[1]] - points[tri_side0->points[0]])^
									(points[tri_side0->points[2]] - points[tri_side0->points[1]]).normalize();
				triangles.push_back(tri_side0);
				Triangle *tri_side1 = new Triangle();
				tri_side1->points[0] = jj + ii*num_of_point;
				tri_side1->points[1] = jj + ii*num_of_point + 1;
				tri_side1->points[2] = jj + ((ii + 1)%radial_slice)*num_of_point + 1;
				tri_side1->normal = (points[tri_side0->points[1]] - points[tri_side0->points[0]])^
									(points[tri_side0->points[2]] - points[tri_side0->points[1]]).normalize();
				triangles.push_back(tri_side1);
			}
		}
	}
}

void Mesh::copyMesh(Mesh *mesh)
{
	mesh_type = mesh->mesh_type;
	extrude_length = mesh->extrude_length;
	radial_slice = mesh->radial_slice;
	num_of_point = mesh->num_of_point;
	height = mesh->height;
	points = mesh->points;
	triangles = mesh->triangles;
	checksum = mesh->checksum;
	status_msg = mesh->status_msg;
}

double Mesh::getHeight() { return height; }

bool Mesh::getChecksum() { return checksum; }

string Mesh::getStatusMsg() { return status_msg; }

bool Mesh::isConvex()
{
	if (points.size() < 2) // Check if it's a polygon
		return false;

	// Check if the turning angle at each point has the same sign
	int turn_angle = 0;
	for (int ii = 0; ii < num_of_point ; ii++) {
		vec3 angle = (points[ii+1] - points[ii])^(points[ii+2] - points[ii+1]);

		if (fabs(angle[1]) > EPSILON) {
			// If it's negative, the cross product of all vertices should all be negative
			if (angle[1] < -EPSILON) {
				if (turn_angle == 1) { return false; }
				turn_angle = -1;
			}
			// If it's positive, the cross product of all vertices should all be positive
			else {
				if (turn_angle == -1) { return false; }
				turn_angle = 1;
			}
		}
	}
	return true;
}