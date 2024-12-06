#pragma once

#include "Transform.h"
#include <list>
#include "types.h"
#include <vector>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

class Camera {

public:
	double fov = glm::radians(60.0);
	double aspect = 16.0 / 9.0 ;
	double zNear = 0.1;
	double zFar = 128.0;

private:
	Transform _transform;

public:
	const auto& transform() const { return _transform; }
	auto& transform() { return _transform; }

	mat4 projection() const;
	mat4 view() const;
	std::list<Plane> frustumPlanes() const;
    void drawFrustum() const {
        auto planes = frustumPlanes();
        std::vector<glm::vec3> frustumCorners = {
            glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1),
            glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1),
            glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1),
            glm::vec3(1, 1, 1), glm::vec3(-1, 1, 1)
        };

        glm::mat4 invProjView = glm::inverse(projection() * view());
        for (auto& corner : frustumCorners) {
            glm::vec4 transformedCorner = invProjView * glm::vec4(corner, 1.0f);
            corner = glm::vec3(transformedCorner) / transformedCorner.w;
        }

        glBegin(GL_LINES);
        for (int i = 0; i < 4; ++i) {
            glVertex3fv(glm::value_ptr(frustumCorners[i]));
            glVertex3fv(glm::value_ptr(frustumCorners[(i + 1) % 4]));
            glVertex3fv(glm::value_ptr(frustumCorners[i + 4]));
            glVertex3fv(glm::value_ptr(frustumCorners[(i + 1) % 4 + 4]));
            glVertex3fv(glm::value_ptr(frustumCorners[i]));
            glVertex3fv(glm::value_ptr(frustumCorners[i + 4]));
        }
        glEnd();
    
    }
	
};


