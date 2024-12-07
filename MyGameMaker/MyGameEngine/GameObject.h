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

class GameObject : public TreeExt<GameObject>, public std::enable_shared_from_this<GameObject>{
private:
    //Transform _transform;                       // Transformaci�n del objeto
    glm::u8vec3 _color = glm::u8vec3(255, 255, 255); // Color del objeto
    Texture _texture;                           // Textura del objeto
    std::shared_ptr<Mesh> _mesh_ptr;           // Puntero a la malla
    //std::vector<std::shared_ptr<Component>> _components; // Lista de componentes
    bool _active = true;                        // Estado de activaci�n
    std::string name;
	mutable bool hasCreatedCheckerTexture = false;		// Indica si la textura de cuadros ha sido creada
    // Restaura la textura original del objeto

    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

    mutable std::type_index cachedComponentType;
    mutable std::shared_ptr<Component> cachedComponent;
	
public:
    GameObject(const std::string& name = "GameObject");
    bool hasCheckerTexture = false;
    // Constructor y destructor
    /*GameObject(const std::string& name = "New GameObject");
    ~GameObject();*/

    //// M�todos para acceder y modificar propiedades
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

	// M�todos para a�adir, obtener y eliminar componentes
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


    // Gesti�n de componentes
    //void addComponent(std::shared_ptr<Component> component);
    //void removeComponent(std::shared_ptr<Component> component);
    //std::vector<std::shared_ptr<Component>> getComponents() const;

    // Transformaci�n global del objeto
    Transform worldTransform() const { return isRoot() ? GetComponent<TransformComponent>()->transform() : parent().worldTransform() * GetComponent<TransformComponent>()->transform(); }

    // C�lculo de las cajas de colisi�n
    BoundingBox localBoundingBox() const; // Definir en el .cpp
    BoundingBox boundingBox() const { return GetComponent<TransformComponent>()->transform().mat() * localBoundingBox(); }
    BoundingBox worldBoundingBox() const; // Definir en el .cpp

    // M�todos para manejar textura y malla
    void setTextureImage(const std::shared_ptr<Image>& img_ptr) { _texture.setImage(img_ptr); }
    void setMesh(const std::shared_ptr<Mesh>& mesh_ptr) { _mesh_ptr = mesh_ptr; }

    // Comprobaciones de existencia de textura y malla
    bool hasTexture() const { return _texture.id(); }
    bool hasMesh() const { return _mesh_ptr != nullptr; }

    // M�todo para dibujar el objeto
    void draw() const; // Definir en el .cpp

    // M�todos de ciclo de vida
    //virtual void awake();      // Inicializaci�n
    //virtual void start();      // Llamado al inicio
    //virtual void update(float deltaTime); // L�gica de actualizaci�n
    //virtual void onDestroy();  // Limpieza antes de eliminar el objeto

    // Activaci�n del objeto
    void setActive(bool active) { _active = active; }
    bool isActive() const { return _active; }

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
        //Log a warning 
    }
}

template <typename T>
bool GameObject::HasComponent() const {
    return components.find(typeid(T)) != components.end();
}

