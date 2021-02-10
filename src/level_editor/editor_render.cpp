#include "editor_render.h"

#include "components/collision/collision.h"
#include "system/world.h"
#include "system/camera.h"
#include "system/system.h"

#include "utils/log.h"

namespace {

const std::array<std::pair<Collideable::CollisionType, sf::Color>, 10> type_color_map = {{
    {Collideable::CollisionType::STATIC, sf::Color(0, 128, 0, 200)},
    {Collideable::CollisionType::SEMI_SOLID, sf::Color(0, 255, 255, 200)},
    {Collideable::CollisionType::SLOPE, sf::Color(0, 0, 128, 200)},
    {Collideable::CollisionType::ENEMY_HITBOX, sf::Color(255, 0, 0, 200)},
    {Collideable::CollisionType::PLAYER_HURTBOX, sf::Color(255, 255, 0, 200)},
    {Collideable::CollisionType::PLAYER_HITBOX, sf::Color(255, 128, 0, 200)},
    {Collideable::CollisionType::PLAYER_DIVE, sf::Color(200, 128, 0, 200)},
    {Collideable::CollisionType::TRANSITION, sf::Color(128, 128, 128, 200)},
    {Collideable::CollisionType::INTERACTABLE, sf::Color(255, 128, 0, 200)},
    {Collideable::CollisionType::COLLECTIBLE, sf::Color(255, 0, 255, 200)},
}};

void renderHitboxes(sf::RenderWindow& window, Collideable::CollisionType coll_type, sf::Color color) {
    auto colls = World::IWorldRead::getCollideables(coll_type);

    for (auto it : colls) {
        if (auto ptr = it.lock()) {
            auto hitbox = ptr->getHitbox();
            auto trans = ptr->getTransform().lock();

            if (!trans) {
                LOGD("EditorRender: unable to lock transform");
                continue;
            }

            sf::RectangleShape rectangle(sf::Vector2f(static_cast<float>(hitbox->width_), static_cast<float>(hitbox->height_)));
            rectangle.setPosition(static_cast<float>(trans->getX() + (hitbox->getLeft())), static_cast<float>(trans->getY() + hitbox->getTop()));
            rectangle.setFillColor(color);
            window.draw(rectangle);
        }
    }
}

void renderEntrances(sf::RenderWindow& window, const std::vector<util::Point>& entrances) {
    constexpr int width = 50.0f;
    constexpr int height = 200.0f;
    for (auto it : entrances) {
        sf::RectangleShape rectangle(sf::Vector2f(width, height));
        rectangle.setPosition(static_cast<float>(it.x - ((width) / 2.0)), static_cast<float>(it.y - (height) / 2.0));
        rectangle.setFillColor(sf::Color(255, 255, 255, 128));
        window.draw(rectangle);
    }
}

void drawLine(float x1, float y1, float x2, float y2, sf::RenderWindow& window) {
    sf::Vertex lines[] = {
        sf::Vertex(sf::Vector2f(x1, y1)),
        sf::Vertex(sf::Vector2f(x2, y2))
    };

    for (auto& it : lines) {
        it.color = sf::Color::Red;
    }

    window.draw(lines, 2, sf::Lines);
}

} // namespace

void EditorRender::render(sf::RenderWindow& window, float frame_fraction) {
    render_.render(window, frame_fraction);

    auto viewport = System::getCamera()->getRawView();
    sf::View view = {{viewport.x_pos, viewport.y_pos}, {viewport.width, viewport.height}};
    window.setView(view);

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

    if (render_entrances_) {
        renderEntrances(window, World::IWorldRead::getEntrances());
    }

    drawCameraBoundaries(window);
}

void EditorRender::drawCameraBoundaries(sf::RenderWindow& window) {
    auto viewport = System::getCamera()->getView();
    sf::View view = {{viewport.x_pos, viewport.y_pos}, {viewport.width, viewport.height}};

    auto view_center = view.getCenter();
    auto view_size = view.getSize();

    auto view_left = view_center.x - (view_size.x / 2.0f);
    auto view_right = view_center.x + (view_size.x / 2.0f);
    auto view_top = view_center.y - (view_size.y / 2.0f);
    auto view_bottom = view_center.y + (view_size.y / 2.0f);

    auto camera = System::getCamera()->getCameraBox();

    window.setView(view);

    drawLine(camera.left_margin, view_top, camera.left_margin, view_bottom, window);
    drawLine(camera.right_margin, view_top, camera.right_margin, view_bottom, window);
    drawLine(view_left, camera.top_margin, view_right, camera.top_margin, window);
    drawLine(view_left, camera.bottom_margin, view_right, camera.bottom_margin, window);
}

void EditorRender::setEditorEnvironment(std::weak_ptr<EditorEnvironment> editor_env) {
    editor_env_ = editor_env;
}

void EditorRender::addEntity(std::weak_ptr<RenderableEntity> entity) {
    render_.addEntity(entity);
}

void EditorRender::setPlayer(std::weak_ptr<RenderableEntity> entity) {
    render_.setPlayer(entity);
}

void EditorRender::renderLayer(sf::RenderWindow& window, int layer) {
    render_.renderLayer(window, 0.0f, layer);
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

void EditorRender::toggleEntranceRendering() {
    render_entrances_ = !render_entrances_;
}

void EditorRender::setCameraBox(Camera::CameraBoundingBox camera_box) {
    if (camera_box.left_margin > camera_box.right_margin ||
        camera_box.top_margin > camera_box.bottom_margin) {
        LOGW("Invalid camera margins");
        return;
    }

    World::IWorldModify::loadCameraData(camera_box);
    System::getCamera()->setCameraBox(camera_box);
}
