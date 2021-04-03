#include "component_store.h"

template <>
std::shared_ptr<Transform> ComponentStore::getComponent() const {
    return transform;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Transform> component) {
    transform = component;
}

template <>
std::shared_ptr<Collision> ComponentStore::getComponent() const {
    return collision;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Collision> component) {
    collision = component;
}

template <>
std::shared_ptr<MovableEntity> ComponentStore::getComponent() const {
    return movableEntity;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<MovableEntity> component) {
    movableEntity = component;
}

template <>
std::shared_ptr<RenderableEntity> ComponentStore::getComponent() const {
    return renderableEntity;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<RenderableEntity> component) {
    renderableEntity = component;
}

template <>
std::shared_ptr<AnimatedEntity> ComponentStore::getComponent() const {
    return animatedEntity;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<AnimatedEntity> component) {
    animatedEntity = component;
}

template <>
std::shared_ptr<StatefulEntity> ComponentStore::getComponent() const {
    return statefulEntity;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<StatefulEntity> component) {
    statefulEntity = component;
}

template <>
std::shared_ptr<Actions> ComponentStore::getComponent() const {
    return actions;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Actions> component) {
    actions = component;
}

template <>
std::shared_ptr<Physics> ComponentStore::getComponent() const {
    return physics;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Physics> component) {
    physics = component;
}

template <>
std::shared_ptr<AI> ComponentStore::getComponent() const {
    return ai;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<AI> component) {
    ai = component;
}

template <>
std::shared_ptr<Damageable> ComponentStore::getComponent() const {
    return damageable;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Damageable> component) {
    damageable = component;
}

template <>
std::shared_ptr<Subentity> ComponentStore::getComponent() const {
    return subentity;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Subentity> component) {
    subentity = component;
}

template <>
std::shared_ptr<Death> ComponentStore::getComponent() const {
    return death;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Death> component) {
    death = component;
}
