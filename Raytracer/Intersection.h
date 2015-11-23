#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "algebra3.h"

// Algebraic Method
double RaySphereAlgebraicIntersection(vec3 const& ro, vec3 const& rd, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal);

// Geometric Method
double RaySphereGeometricIntersection(vec3 const& ro, vec3 const& rd, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal);

// Based on "Fast, Minimum Storage Ray-Triangle Intersection" by Moller and Trumbore
// Can be optimized further: http://www.cs.lth.se/home/Tomas_Akenine_Moller/raytri/
double RayTriangleMollerTrumboreIntersection(vec3 const& ro, vec3 const& rd, vec3 const& p1, vec3 const& p2, vec3 const& p3, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal);

// Slabs method from "Ray Tracing Complex Scenes" by Kay and Kayjia
double RayCubeSlabsIntersection(vec3 const& ro, vec3 const& rd, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal);

// Plucker Coordinates method from "Fast Ray-Axis Aligned Bounding Box Overlap Tests with Plucker Coordinates" by Mahovsky and Wyvill
double RayCubePluckerIntersection(vec3 const& ro, vec3 const& rd, mat4 const& t_inv, mat4 const& tstar_inv, mat4 const& t_trans_inv, vec3& normal);

#endif