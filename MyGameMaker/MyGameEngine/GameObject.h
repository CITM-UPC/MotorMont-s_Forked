#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <glm/glm.hpp>
#include "TreeExt.h"
#include "Transform.h"
#include "Texture.h"
#include "BoundingBox.h"
#include "Mesh.h"
<<<<<<< Updated upstream
#include <string>
#include "CameraComponent.h"
=======
>>>>>>> Stashed changes
#include "Component.h"
#include "TransformComponent.h"
#include <GL/glew.h>
#include "Camera.h"

<<<<<<< Updated upstream



class GameObject : public std::enable_shared_from_this<GameObject>, public TreeExt<GameObject> {
=======
class GameObject : public TreeExt<GameObject> {
>>>>>>> Stashed changes
private:
    glm::u8vec3 _color = glm::u8vec3(255, 255, 255); // Color del objeto
<<<<<<< Updated upstream
    Texture _texture;                           // Textura del objeto
    std::shared_ptr<Mesh> _mesh_ptr;           // Puntero a la malla
    //std::vector<std::shared_ptr<Component>> _components; // Lista de componentes
    bool _active = true;                        // Estado de activación
   
	mutable bool hasCreatedCheckerTexture = false;		// Indica si la textura de cuadros ha sido creada
    // Restaura la textura original del objeto
	
    std::string tag = "Untagged";
    bool active = true;
    bool destroyed = false;

    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
=======
    Texture _texture; // Textura del objeto
    Mesh* _mesh_ptr = nullptr; // Puntero a la malla
    bool _active = true; // Estado de activación
    std::string name;
>>>>>>> Stashed changes

    mutable bool hasCreatedCheckerTexture = false; // Indica si la textura de cuadros ha sido creada
    std::unordered_map<std::type_index, Component*> components;
    mutable std::type_index cachedComponentType;
<<<<<<< Updated upstream
    mutable std::shared_ptr<Component> cachedComponent;
public:
    bool hasCheckerTexture = false;
    std::string name;
    // Constructor y destructor
    GameObject(const std::string& name = "GameObject");
    //~GameObject();

    template <typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args);

    template <typename T>
    std::shared_ptr<T> GetComponent() const;

    template <typename T>
    void RemoveComponent();

    template <typename T>
    bool HasComponent() const;

    // Métodos para acceder y modificar propiedades
=======
    mutable Component* cachedComponent = nullptr;
    std::vector<GameObject*> children;
    GameObject* parent = nullptr;

public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();
    bool hasCheckerTexture = false;

    // Eliminar el constructor de copia y el operador de asignación de copia
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    // Permitir el constructor de movimiento y el operador de asignación de movimiento
    GameObject(GameObject&&) = default;
    GameObject& operator=(GameObject&&) = default;

>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
    // Gestión de componentes
    //void addComponent(std::shared_ptr<Component> component);
    //void removeComponent(std::shared_ptr<Component> component);
    //std::vector<std::shared_ptr<Component>> getComponents() const;

    // Transformación global del objeto
    Transform worldTransform() const { return isRoot() ? GetComponent<TransformComponent>()->transform() : parent().worldTransform() * GetComponent<TransformComponent>()->transform(); }

    // Cálculo de las cajas de colisión
    BoundingBox localBoundingBox() const; // Definir en el .cpp
    BoundingBox boundingBox() const { return GetComponent<TransformComponent>()->transform().mat() * localBoundingBox(); }
    BoundingBox worldBoundingBox() const; // Definir en el .cpp

    // Métodos para manejar textura y malla
    void setTextureImage(const std::shared_ptr<Image>& img_ptr) { _texture.setImage(img_ptr); }
    void setMesh(const std::shared_ptr<Mesh>& mesh_ptr) { _mesh_ptr = mesh_ptr; }

	std:: string GetName() const;
    // Comprobaciones de existencia de textura y malla
    bool hasTexture() const { return _texture.id(); }
    bool hasMesh() const { return _mesh_ptr != nullptr; }

    // Método para dibujar el objeto
    void draw() const; // Definir en el .cpp

    // Métodos de ciclo de vida
    //virtual void awake();      // Inicialización
    //virtual void start();      // Llamado al inicio
    //virtual void update(float deltaTime); // Lógica de actualización
    //virtual void onDestroy();  // Limpieza antes de eliminar el objeto

    // Activación del objeto
    void setActive(bool active) { _active = active; }
    bool isActive() const { return _active; }

=======
    template <typename T, typename... Args>
    T* AddComponent(Args&&... args);

    template <typename T>
    T* GetComponent() const;

    template <typename T>
    void RemoveComponent();

    template <typename T>
    bool HasComponent() const;

    std::string GetName() const;
    void SetName(const std::string& name);
    GameObject* getParent() const;
    void setParent(GameObject* newParent);
    const std::vector<GameObject*>& getChildren() const;
    void addChild(GameObject* child);
    void removeChild(GameObject* child);
    Transform worldTransform() const;
    BoundingBox localBoundingBox() const;
    BoundingBox boundingBox() const;
    BoundingBox worldBoundingBox() const;
    void setTextureImage(Image* img_ptr);
    void setMesh(Mesh* mesh_ptr);
    bool hasTexture() const;
    bool hasMesh() const;
    void draw() const;
    void setActive(bool active);
    bool isActive() const;
>>>>>>> Stashed changes
    void initializeCheckerTexture();
};
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
template <typename T, typename... Args>
T* GameObject::AddComponent(Args&&... args) {
    T* component = new T(this, std::forward<Args>(args)...);
    components[typeid(T)] = component;
    return component;
}

template <typename T>
<<<<<<< Updated upstream
std::shared_ptr<T> GameObject::GetComponent() const {
    if (cachedComponentType == typeid(T) && cachedComponent) {
        return std::static_pointer_cast<T>(cachedComponent);
    }

=======
T* GameObject::GetComponent() const {
>>>>>>> Stashed changes
    auto it = components.find(typeid(T));
    if (it != components.end()) {
        return static_cast<T*>(it->second);
    }
    return nullptr;
}

template <typename T>
void GameObject::RemoveComponent() {
    auto it = components.find(typeid(T));
    if (it != components.end()) {
        delete it->second;
        components.erase(it);
    }
}

template <typename T>
bool GameObject::HasComponent() const {
    return components.find(typeid(T)) != components.end();
}

// Definiciones de las funciones
Transform GameObject::worldTransform() const {
    // Implementación de la función
    return Transform();
}

BoundingBox GameObject::boundingBox() const {
    // Implementación de la función
    return BoundingBox();
}

void GameObject::setTextureImage(Image* img_ptr) {
    _texture.setImage(img_ptr);
}

void GameObject::setMesh(Mesh* mesh_ptr) {
    _mesh_ptr = mesh_ptr;
}

bool GameObject::hasTexture() const {
    return _texture.image() != nullptr;
}

bool GameObject::hasMesh() const {
    return _mesh_ptr != nullptr;
}

void GameObject::setActive(bool active) {
    _active = active;
}

bool GameObject::isActive() const {
    return _active;
}

void GameObject::initializeCheckerTexture() {
    // Implementación de la función
    hasCreatedCheckerTexture = true;
}
