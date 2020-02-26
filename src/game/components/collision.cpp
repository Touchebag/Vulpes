#include "collision.h"

#include "utils/log.h"

namespace {

int getAbsRight(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getX() + hbox->getRight();
}

int getAbsLeft(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getX() + hbox->getLeft();
}

int getAbsTop(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getY() + hbox->getTop();
}

int getAbsBottom(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getY() + hbox->getBottom();
}

bool collidesX(std::shared_ptr<Transform> first_trans, std::shared_ptr<Hitbox> first_hbox,
               std::shared_ptr<Transform> second_trans, std::shared_ptr<Hitbox> second_hbox) {
    return getAbsLeft(first_trans, first_hbox) < getAbsRight(second_trans, second_hbox)
        && getAbsRight(first_trans, first_hbox) > getAbsLeft(second_trans, second_hbox);
}

bool collidesY(std::shared_ptr<Transform> first_trans, std::shared_ptr<Hitbox> first_hbox,
               std::shared_ptr<Transform> second_trans, std::shared_ptr<Hitbox> second_hbox) {
    return getAbsTop(first_trans, first_hbox) < getAbsBottom(second_trans, second_hbox)
        && getAbsBottom(first_trans, first_hbox) > getAbsTop(second_trans, second_hbox);
}

} // namespace

Collision::Collision(std::weak_ptr<Transform> trans, std::weak_ptr<Hitbox> hbox) :
    trans_(trans),
    hbox_(hbox) {
}

void Collision::loadFromJson(nlohmann::json& j) {
}

std::optional<nlohmann::json> Collision::outputToJson() {
    nlohmann::json j;
    return j;
}

bool Collision::collides(std::weak_ptr<Collision> other_entity) {
    if (auto other_ent = other_entity.lock()) {
        auto this_trans = trans_.lock();
        auto this_hbox = hbox_.lock();
        auto other_trans = other_ent->trans_.lock();
        auto other_hbox = other_ent->hbox_.lock();

        if (this_trans && this_hbox && other_trans && other_hbox) {
            return collidesX(this_trans, this_hbox, other_trans, other_hbox)
                && collidesY(this_trans, this_hbox, other_trans, other_hbox);
        }
    }

    return false;
}

std::pair<double, double> Collision::getMaximumMovement(double stepX, double stepY,
        std::shared_ptr<const Collision> other_coll) {
    auto other_trans = other_coll->trans_.lock();
    auto other_hbox = other_coll->hbox_.lock();

    return getMaximumMovement(stepX, stepY, other_trans, other_hbox);
}

std::pair<double, double> Collision::getMaximumMovement(double stepX, double stepY, std::shared_ptr<Transform> other_trans, std::shared_ptr<Hitbox> other_hbox) {
    double retX = stepX, retY = stepY;

    auto this_trans = trans_.lock();
    auto this_hbox = hbox_.lock();

    if (this_trans && this_hbox) {
        // Check if collision after move
        std::shared_ptr<Transform> new_pos = std::make_shared<Transform>();
        new_pos->setPosition(this_trans->getX() + static_cast<int>(stepX), this_trans->getY() + static_cast<int>(stepY));

        bool collides_x = collidesX(new_pos, this_hbox, other_trans, other_hbox);
        bool collides_y = collidesY(new_pos, this_hbox, other_trans, other_hbox);

        if (!(collides_x && collides_y)) {
            return {retX, retY};
        }

        collides_x = collidesX(this_trans, this_hbox, other_trans, other_hbox);
        collides_y = collidesY(this_trans, this_hbox, other_trans, other_hbox);

        // If X direction was already colliding before movement then we are parallel in this direction
        // I.e. do not change speed
        if (!collides_x) {
            if (stepX > 0.0) {
                retX = std::min(stepX, static_cast<double>(getAbsLeft(other_trans, other_hbox) - getAbsRight(this_trans, this_hbox)));
            } else if (stepX < 0.0) {
                retX = std::max(stepX, static_cast<double>(getAbsRight(other_trans, other_hbox) - getAbsLeft(this_trans, this_hbox)));
            }
        }

        // Same for Y
        if (!collides_y) {
            if (stepY > 0.0) {
                retY = std::min(stepY, static_cast<double>(getAbsTop(other_trans, other_hbox) - getAbsBottom(this_trans, this_hbox)));
            } else if (stepY < 0.0) {
                retY = std::max(stepY, static_cast<double>(getAbsBottom(other_trans, other_hbox) - getAbsTop(this_trans, this_hbox)));
            }
        }
    }

    return {retX, retY};
}

