#include "hitbox.h"

void Hitbox::setHitbox(int width, int height) {
    width_ = width;
    height_ = height;
}

void Hitbox::loadFromJson(nlohmann::json& j) {
    width_ = j["width"].get<int>();
    height_= j["height"].get<int>();
}

std::optional<nlohmann::json> Hitbox::outputToJson() {
    nlohmann::json j;

    j["width"] = static_cast<int>(width_);
    j["height"] = static_cast<int>(height_);

    return {j};
}

bool Hitbox::collides(Hitbox& hitbox) {
    return collidesX(hitbox) && collidesY(hitbox);
}

std::pair<double, double> Hitbox::getMaximumMovement(double stepX, double stepY, Hitbox otherHitbox) {
    double retX = stepX, retY = stepY;

    // If X direction was already colliding last step then we are parallel in this direction
    // I.e. do not change speed
    if (!collidesX(otherHitbox)) {
        if (stepX > 0) {
            retX -= getRight() + stepX - otherHitbox.getLeft();
        } else if (stepX < 0) {
            retX += otherHitbox.getRight() - (getLeft() + stepX);
        }
    }

    // Same for Y
    if (!collidesY(otherHitbox)) {
        if (stepY > 0) {
            retY -= getBottom() + stepY - otherHitbox.getTop();
        } else if (stepY < 0) {
            retY += otherHitbox.getBottom() - (getTop() + stepY);
        }
    }

    return {retX, retY};
}

void Hitbox::setOffset(std::pair<int, int> offset) {
    offset_ = offset;
}

void Hitbox::moveOffset(std::pair<int, int> offset) {
    offset_.first += offset.first;
    offset_.second += offset.second;
}

bool Hitbox::collidesX(Hitbox& otherHitbox) {
    return getLeft() < otherHitbox.getRight() && getRight() > otherHitbox.getLeft();
}

bool Hitbox::collidesY(Hitbox& otherHitbox) {
    return getTop() < otherHitbox.getBottom() && getBottom() > otherHitbox.getTop();
}

int Hitbox::getRight() {
    return offset_.first + (width_ / 2);
}

int Hitbox::getLeft() {
    return offset_.first - (width_ / 2);
}

int Hitbox::getTop() {
    return offset_.second - (height_ / 2);
}

int Hitbox::getBottom() {
    return offset_.second + (height_ / 2);
}
