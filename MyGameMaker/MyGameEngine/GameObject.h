#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "TreeExt.h"
#include "Transform.h"
#include "Texture.h"
#include "BoundingBox.h"
#include "Mesh.h"
#include <string>
#include "Component.h"
#include "TransformComponent.h"
#include <unordered_map>
#include <typeindex>

class GameObject : public TreeExt<GameObject>, public std::enable_shared_from_this<GameObject> {
private:
    glm::u8vec3 _color = glm::u8vec3(255, 255, 255); // Color del objeto
    Texture _texture; // Textura del objeto
    std::shared_ptr<Mesh> _mesh_ptr; // Puntero a la malla
    bool _active = true; // Estado de activación
    std::string name;
    mutable bool hasCreatedCheckerTexture = false; // Indica si la textura de cuadros ha sido creada
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
    mutable std::type_index cachedComponentType;
    mutable std::shared_ptr<Component> cachedComponent;
    std::vector<std::shared_ptr<GameObject>> children_;
    std::shared_ptr<GameObject> parent_;

public:
    GameObject(const std::string& name = "GameObject");
    bool hasCheckerTexture = false;

    // Métodos para acceder y modificar propiedades
    const auto& transform() const { return GetComponent<TransformComponent>()->transform(); }
    auto& transform() { return GetComponent<TransformComponent>()->transform(); }

    const auto& color() const { return _color; }
    auto& color() { return _color; }

    const auto& texture() const { return _texture; }
    auto& texture() { return _texture; }

    const auto& mesh() const { return *_mesh_ptr; }
    auto& mesh() { return *_mesh_ptr; }

    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    // Métodos para añadir, obtener y eliminar componentes
    template <typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args);

    template <typename T>
    std::shared_ptr<T> GetComponent() const;

    template <typename T>
    void RemoveComponent();

    template <typename T>
    bool HasComponent() const;

    std::string GetName() const;
    void SetName(const std::string& name);

    // Transformación global del objeto
    Transform worldTransform() const { return isRoot() ? GetComponent<TransformComponent>()->transform() : parent().worldTransform() * GetComponent<TransformComponent>()->transform(); }

    // Cálculo de las cajas de colisión
    BoundingBox localBoundingBox() const;
    BoundingBox boundingBox() const { return GetComponent<TransformComponent>()->transform().mat() * localBoundingBox(); }
    BoundingBox worldBoundingBox() const;

    // Métodos para manejar textura y malla
    void setTextureImage(const std::shared_ptr<Image>& img_ptr) { _texture.setImage(img_ptr); }
    void setMesh(const std::shared_ptr<Mesh>& mesh_ptr) { _mesh_ptr = mesh_ptr; }

    // Comprobaciones de existencia de textura y malla
    bool hasTexture() const { return _texture.id(); }
    bool hasMesh() const { return _mesh_ptr != nullptr; }

    // Método para dibujar el objeto
    void draw() const;

    // Activación del objeto
    void setActive(bool active) { _active = active; }
    bool isActive() const { return _active; }

    // Métodos para cambiar y obtener el padre del GameObject
    void SetParent(std::shared_ptr<GameObject> newParent);
    std::shared_ptr<GameObject> GetParent() const;
    void addChild(std::shared_ptr<GameObject> child);
    void removeChild(GameObject& child);
    const std::vector<std::shared_ptr<GameObject>>& children() const;

    void initializeCheckerTexture();



};

template <typename T, typename... Args>
std::shared_ptr<T> GameObject::AddComponent(Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
    std::shared_ptr<T> newComponent = std::make_shared<T>(weak_from_this(), std::forward<Args>(args)...);
    components[typeid(T)] = newComponent;
    return newComponent;
}

template <typename T>
std::shared_ptr<T> GameObject::GetComponent() const {
    if (this == nullptr) {
        throw std::runtime_error("GameObject instance is null");
    }
    if (cachedComponentType == typeid(T) && cachedComponent) {
        return std::static_pointer_cast<T>(cachedComponent);
    }

    auto it = components.find(typeid(T));
    if (it != components.end()) {
        cachedComponentType = typeid(T);
        cachedComponent = it->second;
        return std::static_pointer_cast<T>(cachedComponent);
    }
    else {
        throw std::runtime_error("Component not found on GameObject: " + this->GetName());
    }
}

template <typename T>
void GameObject::RemoveComponent() {
    auto it = components.find(typeid(T));
    if (it != components.end()) {
        components.erase(it);
    }
    else {
        // Log a warning
    }
}

template <typename T>
bool GameObject::HasComponent() const {
    return components.find(typeid(T)) != components.end();
}


