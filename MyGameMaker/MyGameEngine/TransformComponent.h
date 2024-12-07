#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"
#include "Transform.h"

class TransformComponent : public Component {
public:
    explicit TransformComponent(std::weak_ptr<GameObject> owner) : Component(owner) {}
    const auto& transform() const { return _transform; }
    auto& transform() { return _transform; }

private:
    Transform _transform; // Ensure Transform is defined properly elsewhere
};

