#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"
#include "Camera.h"

<<<<<<< Updated upstream
class CameraComponent : public Component
{
	Camera _camera;
public:
	explicit CameraComponent(std::weak_ptr<GameObject> owner) : Component(owner) {}
	const auto& camera() const { return _camera; }
	auto& camera() { return _camera; }
=======
class CameraComponent : public Component {
    Camera _camera;
public:
    explicit CameraComponent(GameObject* owner) : Component(owner) {}
    const auto& camera() const { return _camera; }
    auto& camera() { return _camera; }
>>>>>>> Stashed changes
};