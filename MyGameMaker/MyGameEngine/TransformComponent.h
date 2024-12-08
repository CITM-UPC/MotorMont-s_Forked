#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"
#include "Transform.h"

class TransformComponent : public Component
{
	Transform  _transform;
public:
<<<<<<< Updated upstream
	explicit TransformComponent(std::weak_ptr<GameObject> owner) : Component(owner) {}
	const auto& transform() const { return _transform; }
	auto& transform() { return _transform; }
=======
    explicit TransformComponent(GameObject* owner) : Component(owner) {}

    const Transform& transform() const { return _transform; }
    Transform& transform() { return _transform; }

private:
    Transform _transform;
>>>>>>> Stashed changes
};
