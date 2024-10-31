#include "GameObject.h"
#include <GL/glew.h>
#include <glm/gtc/epsilon.hpp>

void GameObject::draw() const {
	glPushMatrix();
	glMultMatrixd(_transform.data());
	glColor3ubv(&_color.r);

	if (hasTexture()) {
		glEnable(GL_TEXTURE_2D);
		_texture.bind();
	}

	if (hasMesh()) _mesh_ptr->draw();
	
	if (hasTexture()) glDisable(GL_TEXTURE_2D);

	for (const auto& child : children()) child.draw();

	glPopMatrix();
}

BoundingBox GameObject::localBoundingBox() const {
	if (children().size()) {
		BoundingBox bbox = _mesh_ptr ? _mesh_ptr->boundingBox() : children().front().boundingBox();
		for (const auto& child : children()) bbox = bbox + child.boundingBox();
		return bbox;
	}
	else return _mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox();
}

BoundingBox GameObject::worldBoundingBox() const {
	BoundingBox bbox = worldTransform().mat() * (_mesh_ptr ? _mesh_ptr->boundingBox() : BoundingBox());
	for (const auto& child : children()) bbox = bbox + child.worldBoundingBox();
	return bbox;
}


