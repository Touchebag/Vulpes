#include "component.h"

Component::Component(std::weak_ptr<ComponentStore> components) :
    component_store_(components) {
}
