#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec3.h"

class Triangle {
public:

	Vec3 p[3];

	Triangle(Vec3 v1, Vec3 v2, Vec3 v3) : p{ v1, v2, v3 } {}
};


#endif