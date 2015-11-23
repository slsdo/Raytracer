#include "Intersection.h"

double RaySphereAlgebraicIntersection(vec3 const& ro, vec3 const& rd, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal)
{
	vec3 o = t_inv*ro;
	vec3 d = tstar_inv*rd;
	vec3 dn = d.normalized();

	double t = -1.0;

	vec3 center(0.0, 0.0, 0.0);
	double r = 1.0;

	double a = dn*dn;
	double b = 2*((o - center)*dn);
	double c = (o - center)*(o - center) - r*r;

	// First check the discriminant
	double delta = b*b - 4*a*c;
	// If it's negative, we know there is no intersection
	if(delta < 0 || fabs(a) < EPSILON)
		return t;

	double root = sqrt(delta); // Now we get the square root
	double t0= (-b - root)/(2*a); // Get the cases for minus
	double t1= (-b + root)/(2*a); // Get the case for plus

	if (t0 > EPSILON && (t1 < EPSILON || t0 < t1)) // If t0 is positive
		t = t0;
	else if (t1 > EPSILON && (t0 < EPSILON || t1 < t0)) // If t1 is positive
		t = t1;
	else // If both are negative
		return t;
		
	// Get the normal in world coordinate
	vec4 n4o(o + t*dn);
	vec4 n4w(t_trans_inv*n4o);
	vec3 n3w(n4w, 3);
	normal = n3w.normalize();
	
	t = t/d.length();
	return t;
}

double RaySphereGeometricIntersection(vec3 const& ro, vec3 const& rd, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal)
{
	vec3 o = t_inv*ro;
	vec3 d = tstar_inv*rd;
	vec3 dn = d.normalized();
	
	double t = -1.0;

	double r = 1;
	double r2 = r*r; // Radius squared
	vec3 c = vec3(0.0, 0.0, 0.0);
	
	vec3 l = c - o; // Vector from ray origin to center of sphere
	double s = l*dn; // Projection of l onto the ray direction
	double l2 = l*l; // Squared length of the vector

	// If sphere is behind the ray
	if (s < EPSILON && l2 > r2)
		return t;

	// Squared distance from sphere center to the projection
	double m2 = l2 - s*s;
	
	// If ray does not touch the sphere
	if (m2 > r2)
		return t;

	double q = sqrt(r2 - m2); // Squared distance

	if (l2 > r2) // If origin is outside the sphere
		t = s - q;
	else // If origin is inside the sphere
		t = s + q;

	// Get the normal in world coordinate
	vec4 n4o(o + t*dn);
	vec4 n4w(t_trans_inv*n4o);
	vec3 n3w(n4w, 3);
	normal = n3w.normalize();
	
	t = t/d.length();
	return t;
}

double RayTriangleMollerTrumboreIntersection(vec3 const& ro, vec3 const& rd, vec3 const& p1, vec3 const& p2, vec3 const& p3, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal)
{
	vec3 o = t_inv*ro;
	vec3 d = tstar_inv*rd;
	vec3 dn = d.normalized();
	
	double t = -1.0;

	// Initialize the barycentric coordinates
	double u = 0.0, v = 0.0;

	// Get the edge vectors of the triangle
	vec3 e1 = p2 - p1;
	vec3 e2 = p3 - p1;

	// Calculate the vectors
	vec3 s = o - p1;
	vec3 p = dn^e2;
	vec3 q = s^e1;

	double det = p*e1;

	// Test the bounds
	if (det > -EPSILON && det < 1e-5)
		return t;

	double inv_det = 1.0/det;

	// Calculate the u parameter
	u = (p*s)*inv_det;
	if (u < EPSILON || u > 1.0)
		return t;

	// Calculate the v parameter
	v = (q*dn)*inv_det;
	if (v < EPSILON || u + v > 1.0)
		return t;

	// If we pass all of the above the tests, then find t, which is the intersection
	t = (q*e2)*inv_det;
	if (t < 1e-5)
		return t;

	// Get the normal in world coordinate
	vec4 n4o((p2 - p1)^(p3 - p1));
	vec4 n4w(t_trans_inv*n4o);
	vec3 n3w(n4w, 3);
	normal = n3w.normalize();
	
	t = t/d.length();
	return t;
}

double RayCubeSlabsIntersection(vec3 const& ro, vec3 const& rd, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal)
{
	vec3 o = t_inv*ro;
	vec3 d = tstar_inv*rd;
	vec3 dn = d.normalized();
	
	double t = -1.0;

	double t_min = -100000;
	double t_max = 100000;
	int nside; // To check which side the normal is pointing out of

	vec3 ac = vec3(0, 0, 0);
	vec3 p = ac - o;

	for (int ii = 0; ii < 3; ii++) {
		double e = p[ii];
		double f = dn[ii];
		// Check if ray direction is perpendicular to the normal direction of the slab
		if (fabs(f) > EPSILON) {
			double fdiv = 1/f; // Division optimization
			double t1 = (e + 0.5)*fdiv;
			double t2 = (e - 0.5)*fdiv;
			// Ensures the minimum of t1 and t2 is stored in t1
			if (t1 > t2) swap(t1, t2);
			if (t1 > t_min) { t_min = t1; nside = (ii + 1); }
			if (t2 < t_max) { t_max = t2; }
			if (t_min > t_max || t_max < 0) return t; // Ray misses the box or box is behind ray
		}
		// If ray is parallel to the slab
		else if (-e - 0.5 > 0 || -e + 0.5 < 0) return t;
	}
	
	if (t_min > EPSILON) { t = t_min/d.length(); nside *= 1; }
	else { t = t_max/d.length(); nside *= -1; }
	
	// Get the normal in world coordinate
	vec3 n3o(0, 0, 0);
	switch (nside) {
		case  1: n3o = vec3( 1, 0, 0); break;
		case -1: n3o = vec3(-1, 0, 0); break;
		case  2: n3o = vec3(0,  1, 0); break;
		case -2: n3o = vec3(0, -1, 0); break;
		case  3: n3o = vec3(0, 0,  1); break;
		case -3: n3o = vec3(0, 0, -1); break;
		default: break;
	}
	vec4 n4o(n3o);
	vec4 n4w(t_trans_inv*n4o);
	vec3 n3w(n4w, 3);
	normal = n3w.normalize();

	return t;
}

double RayCubePluckerIntersection(vec3 const& ro, vec3 const& rd, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal)
{
	vec3 o = t_inv*ro;
	vec3 d = tstar_inv*rd;
	vec3 dn = d.normalized();
	
	double t = -1.0;

	if(dn[0] < 0) {
		if(dn[1] < 0) {
			if(dn[2] < 0) {
				// case MMM: side(R,HD) < 0 or side(R,FB) > 0 or side(R,EF) > 0 or side(R,DC) < 0 or side(R,CB) < 0 or side(R,HE) > 0 to miss

				if ((o[0] < -0.5) || (o[1] < -0.5) || (o[2] < -0.5))
					return -1;

				double xa = -0.5 - o[0]; 
				double ya = -0.5 - o[1]; 
				double za = -0.5 - o[2]; 
				double xb = 0.5 - o[0];
				double yb = 0.5 - o[1];
				double zb = 0.5 - o[2];

				if(	(dn[0]*ya - dn[1]*xb < 0) ||
					(dn[0]*yb - dn[1]*xa > 0) ||
					(dn[0]*zb - dn[2]*xa > 0) ||
					(dn[0]*za - dn[2]*xb < 0) ||
					(dn[1]*za - dn[2]*yb < 0) ||
					(dn[1]*zb - dn[2]*ya > 0))
					return -1;

				// compute the intersection distance

				t = xb/dn[0];
				double t1 = yb/dn[1];
				if(t1 > t)
					t = t1;
				double t2 = zb/dn[2];
				if(t2 > t)
					t = t2;

				return t/d.length();
			}
			else {
				// case MMP: side(R,HD) < 0 or side(R,FB) > 0 or side(R,HG) > 0 or side(R,AB) < 0 or side(R,DA) < 0 or side(R,GF) > 0 to miss

				if ((o[0] < -0.5) || (o[1] < -0.5) || (o[2] > 0.5))
					return -1;

				double xa = -0.5 - o[0]; 
				double ya = -0.5 - o[1]; 
				double za = -0.5 - o[2]; 
				double xb = 0.5 - o[0];
				double yb = 0.5 - o[1];
				double zb = 0.5 - o[2];

				if(	(dn[0]*ya - dn[1]*xb < 0) ||
					(dn[0]*yb - dn[1]*xa > 0) ||
					(dn[0]*zb - dn[2]*xb > 0) ||
					(dn[0]*za - dn[2]*xa < 0) ||
					(dn[1]*za - dn[2]*ya < 0) ||
					(dn[1]*zb - dn[2]*yb > 0))
					return -1;

				t = xb/dn[0];
				double t1 = yb/dn[1];
				if(t1 > t)
					t = t1;
				double t2 = za/dn[2];
				if(t2 > t)
					t = t2;

				return t/d.length();
			}
		} 
		else {
			if(dn[2] < 0) {
				// case MPM: side(R,EA) < 0 or side(R,GC) > 0 or side(R,EF) > 0 or side(R,DC) < 0 or side(R,GF) < 0 or side(R,DA) > 0 to miss

				if ((o[0] < -0.5) || (o[1] > 0.5) || (o[2] < -0.5))
					return -1;

				double xa = -0.5 - o[0]; 
				double ya = -0.5 - o[1]; 
				double za = -0.5 - o[2]; 
				double xb = 0.5 - o[0];
				double yb = 0.5 - o[1];
				double zb = 0.5 - o[2];

				if(	(dn[0]*ya - dn[1]*xa < 0) ||
					(dn[0]*yb - dn[1]*xb > 0) ||
					(dn[0]*zb - dn[2]*xa > 0) ||
					(dn[0]*za - dn[2]*xb < 0) ||
					(dn[1]*zb - dn[2]*yb < 0) ||
					(dn[1]*za - dn[2]*ya > 0))
					return -1;

				t = xb/dn[0];
				double t1 = ya/dn[1];
				if(t1 > t)
					t = t1;
				double t2 = zb/dn[2];
				if(t2 > t)
					t = t2;

				return t/d.length();
			}
			else {
				// case MPP: side(R,EA) < 0 or side(R,GC) > 0 or side(R,HG) > 0 or side(R,AB) < 0 or side(R,HE) < 0 or side(R,CB) > 0 to miss

				if ((o[0] < -0.5) || (o[1] > 0.5) || (o[2] > 0.5))
					return -1;

				double xa = -0.5 - o[0]; 
				double ya = -0.5 - o[1]; 
				double za = -0.5 - o[2]; 
				double xb = 0.5 - o[0];
				double yb = 0.5 - o[1];
				double zb = 0.5 - o[2];

				if(	(dn[0]*ya - dn[1]*xa < 0) ||
					(dn[0]*yb - dn[1]*xb > 0) ||
					(dn[0]*zb - dn[2]*xb > 0) ||
					(dn[0]*za - dn[2]*xa < 0) ||
					(dn[1]*zb - dn[2]*ya < 0) ||
					(dn[1]*za - dn[2]*yb > 0))
					return -1;

				t = xb/dn[0];
				double t1 = ya/dn[1];
				if(t1 > t)
					t = t1;
				double t2 = za/dn[2];
				if(t2 > t)
					t = t2;

				return t/d.length();
			}
		}
	}
	else {
		if(dn[1] < 0) {
			if(dn[2] < 0) {
				// case PMM: side(R,GC) < 0 or side(R,EA) > 0 or side(R,AB) > 0 or side(R,HG) < 0 or side(R,CB) < 0 or side(R,HE) > 0 to miss

				if ((o[0] > 0.5) || (o[1] < -0.5) || (o[2] < -0.5))
					return -1;

				double xa = -0.5 - o[0]; 
				double ya = -0.5 - o[1]; 
				double za = -0.5 - o[2]; 
				double xb = 0.5 - o[0];
				double yb = 0.5 - o[1];
				double zb = 0.5 - o[2];

				if(	(dn[0]*yb - dn[1]*xb < 0) ||
					(dn[0]*ya - dn[1]*xa > 0) ||
					(dn[0]*za - dn[2]*xa > 0) ||
					(dn[0]*zb - dn[2]*xb < 0) ||
					(dn[1]*za - dn[2]*yb < 0) ||
					(dn[1]*zb - dn[2]*ya > 0))
					return -1;

				t = xa/dn[0];
				double t1 = yb/dn[1];
				if(t1 > t)
					t = t1;
				double t2 = zb/dn[2];
				if(t2 > t)
					t = t2;

				return t/d.length();
			}
			else {
				// case PMP: side(R,GC) < 0 or side(R,EA) > 0 or side(R,DC) > 0 or side(R,EF) < 0 or side(R,DA) < 0 or side(R,GF) > 0 to miss

				if ((o[0] > 0.5) || (o[1] < -0.5) || (o[2] > 0.5))
					return -1;

				double xa = -0.5 - o[0]; 
				double ya = -0.5 - o[1]; 
				double za = -0.5 - o[2]; 
				double xb = 0.5 - o[0];
				double yb = 0.5 - o[1];
				double zb = 0.5 - o[2];

				if(	(dn[0]*yb - dn[1]*xb < 0) ||
					(dn[0]*ya - dn[1]*xa > 0) ||
					(dn[0]*za - dn[2]*xb > 0) ||
					(dn[0]*zb - dn[2]*xa < 0) ||
					(dn[1]*za - dn[2]*ya < 0) ||
					(dn[1]*zb - dn[2]*yb > 0))
					return -1;

				t = xa/dn[0];
				double t1 = yb/dn[1];
				if(t1 > t)
					t = t1;
				double t2 = za/dn[2];
				if(t2 > t)
					t = t2;

				return t/d.length();
			}
		}
		else {
			if(dn[2] < 0) {
				// case PPM: side(R,FB) < 0 or side(R,HD) > 0 or side(R,AB) > 0 or side(R,HG) < 0 or side(R,GF) < 0 or side(R,DA) > 0 to miss

				if ((o[0] > 0.5) || (o[1] > 0.5) || (o[2] < -0.5))
					return -1;

				double xa = -0.5 - o[0]; 
				double ya = -0.5 - o[1]; 
				double za = -0.5 - o[2]; 
				double xb = 0.5 - o[0];
				double yb = 0.5 - o[1];
				double zb = 0.5 - o[2];

				if(	(dn[0]*yb - dn[1]*xa < 0) ||
					(dn[0]*ya - dn[1]*xb > 0) ||
					(dn[0]*za - dn[2]*xa > 0) ||
					(dn[0]*zb - dn[2]*xb < 0) ||
					(dn[1]*zb - dn[2]*yb < 0) ||
					(dn[1]*za - dn[2]*ya > 0))
					return -1;

				t = xa/dn[0];
				double t1 = ya/dn[1];
				if(t1 > t)
					t = t1;
				double t2 = zb/dn[2];
				if(t2 > t)
					t = t2;

				return t/d.length();
			}
			else {
				// case PPP: side(R,FB) < 0 or side(R,HD) > 0 or side(R,DC) > 0 or side(R,EF) < 0 or side(R,HE) < 0 or side(R,CB) > 0 to miss

				if ((o[0] > 0.5) || (o[1] > 0.5) || (o[2] > 0.5))
					return -1;

				double xa = -0.5 - o[0]; 
				double ya = -0.5 - o[1]; 
				double za = -0.5 - o[2]; 
				double xb = 0.5 - o[0];
				double yb = 0.5 - o[1];
				double zb = 0.5 - o[2];

				if(	(dn[0]*yb - dn[1]*xa < 0) ||
					(dn[0]*ya - dn[1]*xb > 0) ||
					(dn[0]*za - dn[2]*xb > 0) ||
					(dn[0]*zb - dn[2]*xa < 0) ||
					(dn[1]*zb - dn[2]*ya < 0) ||
					(dn[1]*za - dn[2]*yb > 0))
					return -1;

				t = xa/dn[0];
				double t1 = ya/dn[1];
				if(t1 > t)
					t = t1;
				double t2 = za/dn[2];
				if(t2 > t)
					t = t2;

				return t/d.length();
			}
		}
	}

	return -1;
}