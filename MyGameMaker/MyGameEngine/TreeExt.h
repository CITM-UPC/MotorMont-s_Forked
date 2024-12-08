#pragma once

#include <list>
#include "readOnlyView.h"

template <class T>
class TreeExt {
private:
    T* _parent = nullptr;
    std::list<T*> _children;

public:
    T* parent() const { return _parent; }
    const std::list<T*>& children() const { return _children; }

    T& root() const { return _parent ? _parent->root() : *this; }
    bool isRoot() const { return !_parent; }

    template <typename ...Args>
    T& emplaceChild(Args&&... args) {
        T* child = new T(std::forward<Args>(args)...);
        child->_parent = static_cast<T*>(this);
        _children.push_back(child);
        return *child;
    }

    void removeChild(T* child) {
        _children.remove(child);
        delete child;
    }

    void setParent(T* parent) { _parent = parent; }

    // Destructor para limpiar los hijos
    ~TreeExt() {
        for (T* child : _children) {
            delete child;
        }
    }

    // Eliminar el constructor de copia y el operador de asignación de copia
    TreeExt(const TreeExt&) = delete;
    TreeExt& operator=(const TreeExt&) = delete;

    // Permitir el constructor de movimiento y el operador de asignación de movimiento
    TreeExt(TreeExt&&) = default;
    TreeExt& operator=(TreeExt&&) = default;
};
