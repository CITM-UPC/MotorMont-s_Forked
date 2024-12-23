#pragma once

#include <glm/glm.hpp>

using mat4 = glm::dmat4;
using vec4 = glm::dvec4;
using vec3 = glm::dvec3;

namespace Colors {
	const glm::u8vec3 Red( 255, 0, 0);
	const glm::u8vec3 Green( 0, 255, 0);
	const glm::u8vec3 Blue( 0, 0, 255);
};

struct Point {
	vec3 p;
	Point(const vec3& v3) : p(v3) {}
};

inline Point operator*(const mat4& m, const Point& p) {
	const auto v3 = m * vec4(p.p, 1);
	return Point(vec3(v3) / v3.w);
}

struct Vector {
	vec3 v;
	Vector(const vec3& v3) : v(v3) {}
};

inline Vector operator*(const mat4& m, const Vector& v) { return Vector(vec3(m * vec4(v.v, 0))); }

struct Plane {
	glm::vec3 normal; 
	glm::vec3 point;  

	Plane(const glm::vec3& normal, const glm::vec3& point)
		: normal(glm::normalize(normal)), point(point) {}

	float distance(const glm::vec3& p) const {
		return glm::dot(normal, p - point);
	}
};

inline Plane operator*(const glm::mat4& m, const Plane& plane) {

	glm::vec3 transformedNormal = glm::normalize(glm::vec3(m * glm::vec4(plane.normal, 0.0f)));

	glm::vec3 transformedPoint = glm::vec3(m * glm::vec4(plane.point, 1.0f));

	return Plane(transformedNormal, transformedPoint);
}
