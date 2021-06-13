#include "component.h"

#include "component_store.h"

Component::Component(std::weak_ptr<ComponentStore> components) :
    component_store_(components) {
}

template <typename T>
std::shared_ptr<T> Component::getComponent() const {
    if (auto store = component_store_.lock()) {
        return store->getComponent<T>();
    } else {
        return {};
    }
}

template std::shared_ptr<Transform> Component::getComponent() const;
template std::shared_ptr<Collision> Component::getComponent() const;
template std::shared_ptr<MovableEntity> Component::getComponent() const;
template std::shared_ptr<RenderableEntity> Component::getComponent() const;
template std::shared_ptr<AnimatedEntity> Component::getComponent() const;
template std::shared_ptr<StatefulEntity> Component::getComponent() const;
template std::shared_ptr<Actions> Component::getComponent() const;
template std::shared_ptr<Physics> Component::getComponent() const;
template std::shared_ptr<AI> Component::getComponent() const;
template std::shared_ptr<Damageable> Component::getComponent() const;
template std::shared_ptr<Subentity> Component::getComponent() const;
template std::shared_ptr<Death> Component::getComponent() const;
