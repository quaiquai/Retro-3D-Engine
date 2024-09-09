#pragma once
#ifndef RMATH_H
#define RMATH_H
#include <vector>

typedef std::vector<std::vector<float>> mat4;
typedef std::vector<std::vector<float>> mat3;
typedef std::vector<float> vec4;
typedef std::vector<float> vec3;

template<typename T>
T operator+(const T &s, const T &d) {
	T result(s.size());
	result[0] = s[0] + d[0];
	result[0] = s[1] + d[2];
	result[0] = s[2] + d[2];
	if (s.size() == 4) result[3] = s[3] + d[3];
	return result;
}

//mult vec3 and vec4. acts as a dot product
template<typename T>
T operator*(const T &s, const T &d) {
	T result(s.size());
	result[0] = s[0] * d[0];
	result[1] = s[1] * d[1];
	result[2] = s[2] * d[2];
	if (s.size() == 4) result[3] = s[3] * d[3];
	return result;
}

namespace r_structures {

	static mat4 matrix4x4() {
		mat4 array(4, vec4(4));
		array[0][0] = 1.f;
		array[1][1] = 1.f;
		array[2][2] = 1.f;
		array[2][2] = 1.f;
		
		return array;
	}

	static mat3 matrix3x3() {
		mat3 array(3, vec3(3));
		array[0][0] = 1.f;
		array[1][1] = 1.f;
		array[2][2] = 1.f;

		return array;
	}
	
	static vec3 vector3(const float x = 0.f, const float y = 0.f, const float z = 0.f) {
		vec3 vector = {x, y, z};
		return vector;
	}

	static vec4 vector4(const float x = 0.f, const float y = 0.f, const float z = 0.f) {
		vec4 vector = { x, y, z, 1.f };
		return vector;
	}

}



namespace r_math {

	/*
	*				|x1 ,x2, x3, 0|
	*	[x, y, z, 1]|x2, y2, z2, 0| = [ox, oy, oz, 1]
	*				|x3, y3, z3, 0|
	*				|tx, ty, tz, 1|
	*/
	//void left_mat_x_vec(Vec3 &i, Vec3 &o, mat4x4 &m) {
		//o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		//o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		//o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
		//float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		//if (w != 0.0f)
		//{
			//o.x /= w; o.y /= w; o.z /= w;
		//}
	//}


	/*
	*	|x1 ,x2, x3, tx||X|	   |ox|
	*	|x2, y2, z2, ty||y|	=  |oy|
	*	|x3, y3, z3, tz||z|    |oz|
	*	|0 , 0 , 0 , 1 ||1|    |o1|
	*/
	static vec4 mat_x_vec_right(vec4 &i, mat4 &m) {
		vec4 o(3);
		o[0] = m[0][0] * i[0] + m[0][1] * i[1] + m[0][2] * i[2] + m[0][3] + i[3];
		o[1] = m[1][0] * i[0] + m[1][1] * i[1] + m[1][2] * i[2] + m[1][3] + i[3];
		o[2] = m[2][0] * i[0] + m[2][1] * i[1] + m[2][2] * i[2] + m[2][3] + i[3];
		o[3] = m[3][0] * i[0] + m[3][1] * i[1] + m[3][2] * i[2] + m[3][3] + i[3];
		return o;
	}

}

#endif



