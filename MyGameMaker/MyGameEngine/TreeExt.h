#pragma once

#include <list>
#include "readOnlyView.h"

template <class T>
class TreeExt {

private:
	T* _parent = nullptr;
	std::list<std::shared_ptr<T>> _children;

public:
	auto& parent() const { return *_parent; }
	auto children() const { return readOnlyListView<std::shared_ptr<T>>(_children); }

	auto& root() const { return _parent ? _parent->root() : *this; }
	bool isRoot() const { return !_parent; }

	template <typename ...Args>
	auto& emplaceChild(Args&&... args) {
		auto child = std::make_shared<T>(std::forward<Args>(args)...);
		_children.push_back(child);
		child->_parent = static_cast<T*>(this);
		return *child;
	}

	void removeChild(const T& child) { _children.remove_if([&](const std::shared_ptr<T>& ptr) { return ptr.get() == &child; }); }
};
