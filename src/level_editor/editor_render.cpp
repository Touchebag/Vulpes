#include "editor_render.h"

#include "components/collision/collision.h"
#include "system/world.h"

#include "utils/log.h"

namespace {

const std::array<std::pair<Collision::CollisionType, sf::Color>, 9> type_color_map = {{
    {Collision::CollisionType::STATIC, sf::Color(0, 128, 0, 200)},
    {Collision::CollisionType::SEMI_SOLID, sf::Color(0, 255, 255, 200)},
    {Collision::CollisionType::SLOPE, sf::Color(0, 0, 128, 200)},
    {Collision::CollisionType::ENEMY_HITBOX, sf::Color(255, 0, 0, 200)},
    {Collision::CollisionType::PLAYER_HURTBOX, sf::Color(255, 255, 0, 200)},
    {Collision::CollisionType::PLAYER_HITBOX, sf::Color(255, 128, 0, 200)},
    {Collision::CollisionType::TRANSITION, sf::Color(128, 128, 128, 200)},
    {Collision::CollisionType::INTERACTABLE, sf::Color(255, 128, 0, 200)},
    {Collision::CollisionType::COLLECTIBLE, sf::Color(255, 0, 255, 200)},
}};

void renderHitboxes(sf::RenderWindow& window, Collision::CollisionType coll_type, sf::Color color) {
    auto colls = World::IWorldRead::getCollisions(coll_type);

    for (auto it : colls) {
        if (auto ptr = it.lock()) {
            auto hitbox = ptr->getHitbox();
            auto trans = ptr->getTransform().lock();

            if (!trans) {
                LOGD("EditorRender: unable to lock transform");
                continue;
            }

            sf::RectangleShape rectangle(sf::Vector2f(static_cast<float>(hitbox->width_), static_cast<float>(hitbox->height_)));
            rectangle.setPosition(static_cast<float>(trans->getX() - ((hitbox->width_) / 2)), static_cast<float>(trans->getY() - (hitbox->height_) / 2));
            rectangle.setFillColor(color);
            window.draw(rectangle);
        }
    }
}

} // namespace

void EditorRender::render(sf::RenderWindow& window) {
    render_.render(window);

    if (auto env = editor_env_.lock()) {
        if (auto ent = env->current_entity) {
            std::pair<float, float> size = ent->renderableEntity_->getScaledSize();
            auto trans = ent->trans_;
            sf::RectangleShape rectangle(sf::Vector2f(size.first, size.second));
            rectangle.setPosition(static_cast<float>(trans->getX()) - (size.first / 2.0f), static_cast<float>(trans->getY()) - (size.second / 2.0f));
            rectangle.setFillColor(sf::Color(255, 255, 255, 64));
            window.draw(rectangle);
        }
    }

    if (render_hitboxes_) {
        for (auto it : type_color_map) {
            renderHitboxes(window, it.first, it.second);
        }
    }
}

void EditorRender::setEditorEnvironment(std::weak_ptr<EditorEnvironment> editor_env) {
    editor_env_ = editor_env;
}

void EditorRender::addEntity(std::weak_ptr<RenderableEntity> entity) {
    render_.addEntity(entity);
}

void EditorRender::moveView(float x, float y) {
    render_.moveView(x, y);
}

void EditorRender::resizeView(float width, float height) {
    render_.resizeView(width, height);
}

void EditorRender::setView(float x, float y, float width, float height) {
    render_.setView(x, y, width, height);
}

sf::View EditorRender::getView() {
    return render_.getView();
}

void EditorRender::renderLayer(sf::RenderWindow& window, RenderableEntity::Layer layer) {
    render_.renderLayer(window, layer);
}

void EditorRender::setParallaxEnabled(bool enable) {
    render_.parallax_enabled_ = enable;
}

bool EditorRender::getParallaxEnabled() {
    return render_.parallax_enabled_;
}

void EditorRender::toggleHitboxRendering() {
    render_hitboxes_ = !render_hitboxes_;
}

void EditorRender::setCameraBox(CameraBox camera_box) {
    render_.setCameraBox(camera_box);
}
