#pragma once

inline int getAbsRight(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getX() + hbox->getRight();
}

inline int getAbsLeft(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getX() + hbox->getLeft();
}

inline int getAbsTop(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getY() + hbox->getTop();
}

inline int getAbsBottom(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getY() + hbox->getBottom();
}

inline bool collidesX(std::shared_ptr<const Transform> first_trans, std::shared_ptr<const Hitbox> first_hbox,
                      std::shared_ptr<const Transform> second_trans, std::shared_ptr<const Hitbox> second_hbox) {
    return getAbsLeft(first_trans, first_hbox) < getAbsRight(second_trans, second_hbox)
        && getAbsRight(first_trans, first_hbox) > getAbsLeft(second_trans, second_hbox);
}

inline bool collidesY(std::shared_ptr<const Transform> first_trans, std::shared_ptr<const Hitbox> first_hbox,
                      std::shared_ptr<const Transform> second_trans, std::shared_ptr<const Hitbox> second_hbox) {
    return getAbsTop(first_trans, first_hbox) < getAbsBottom(second_trans, second_hbox)
        && getAbsBottom(first_trans, first_hbox) > getAbsTop(second_trans, second_hbox);
}
