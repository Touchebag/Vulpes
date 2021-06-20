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
std::shared_ptr<Movement> ComponentStore::getComponent() const {
    return movement;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Movement> component) {
    movement = component;
}

template <>
std::shared_ptr<Rendering> ComponentStore::getComponent() const {
    return rendering;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Rendering> component) {
    rendering = component;
}

template <>
std::shared_ptr<Animation> ComponentStore::getComponent() const {
    return animation;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Animation> component) {
    animation = component;
}

template <>
std::shared_ptr<Stateful> ComponentStore::getComponent() const {
    return stateful;
}
template <>
void ComponentStore::setComponent(std::shared_ptr<Stateful> component) {
    stateful = component;
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
