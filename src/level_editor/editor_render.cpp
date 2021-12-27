#include "editor_render.h"

#include "components/collision/collision.h"
#include "system/world.h"
#include "system/camera.h"
#include "system/system.h"

#include "utils/log.h"

namespace {

const std::array<std::pair<Collideable::CollisionType, sf::Color>, 11> type_color_map = {{
    {Collideable::CollisionType::STATIC, sf::Color(0, 128, 0, 200)},
    {Collideable::CollisionType::SEMI_SOLID, sf::Color(0, 255, 255, 200)},
    {Collideable::CollisionType::SLOPE, sf::Color(0, 0, 128, 200)},
    {Collideable::CollisionType::PLAYER_HURTBOX, sf::Color(255, 255, 0, 200)},
    {Collideable::CollisionType::PLAYER_HITBOX, sf::Color(255, 128, 0, 200)},
    {Collideable::CollisionType::PLAYER_DIVE, sf::Color(200, 128, 0, 200)},
    {Collideable::CollisionType::ENEMY_HITBOX, sf::Color(255, 0, 0, 200)},
    {Collideable::CollisionType::TRANSITION, sf::Color(128, 128, 128, 200)},
    {Collideable::CollisionType::COLLECTIBLE, sf::Color(255, 0, 255, 200)},
    {Collideable::CollisionType::INTERACTABLE, sf::Color(255, 128, 0, 200)},
    {Collideable::CollisionType::SENSOR, sf::Color(128, 0, 128, 200)},
}};

void renderSingleHitbox(sf::RenderTarget& target, std::shared_ptr<const Collideable> coll, sf::Color color) {
    auto hitbox = coll->getHitbox();
    auto trans = coll->getTransform().lock();

    if (!trans) {
        LOGD("EditorRender: unable to lock transform");
        return;
    }

    sf::RectangleShape rectangle(sf::Vector2f(static_cast<float>(hitbox->width_), static_cast<float>(hitbox->height_)));
    rectangle.setPosition(static_cast<float>(trans->getX() + (hitbox->getLeft())), static_cast<float>(trans->getY() + hitbox->getTop()));
    rectangle.setFillColor(color);
    target.draw(rectangle);
}

void renderHitboxes(sf::RenderTarget& target, Collideable::CollisionType coll_type, sf::Color color) {
    auto colls = System::IWorldRead::getCollideables(coll_type);

    for (auto it : colls) {
        if (auto ptr = it.lock()) {
            renderSingleHitbox(target, ptr, color);
        }
    }
}

void renderEntrances(sf::RenderTarget& target, const std::vector<util::Vec2i>& entrances) {
    constexpr int width = 50.0f;
    constexpr int height = 200.0f;
    for (auto it : entrances) {
        sf::RectangleShape rectangle(sf::Vector2f(width, height));
        rectangle.setPosition(static_cast<float>(it.x - ((width) / 2.0)), static_cast<float>(it.y - (height) / 2.0));
        rectangle.setFillColor(sf::Color(255, 255, 255, 128));
        target.draw(rectangle);
    }
}

void drawLine(float x1, float y1, float x2, float y2, sf::RenderTarget& target) {
    sf::Vertex lines[] = {
        sf::Vertex(sf::Vector2f(x1, y1)),
        sf::Vertex(sf::Vector2f(x2, y2))
    };

    for (auto& it : lines) {
        it.color = sf::Color::Red;
    }

    target.draw(lines, 2, sf::Lines);
}

} // namespace

void EditorRender::render(sf::RenderTarget& window, float frame_fraction) {
    render_.render(window, frame_fraction);

    auto camera = System::getCamera();

    auto viewport = camera->getRawView();
    sf::View view = {{viewport.x_pos, viewport.y_pos}, {viewport.width, viewport.height}};

    auto window_size = camera->getWindowSize();
    sf::RenderTexture texture;
    texture.create(window_size.first, window_size.second);
    texture.clear(sf::Color(0, 0, 0, 0));
    texture.setView(view);

    if (auto env = editor_env_.lock()) {
        if (auto ent = env->current_entity) {
            auto rend = ent->getComponent<Rendering>();
            auto trans = ent->getComponent<Transform>();

            if (rend && trans) {
                std::pair<float, float> size = ent->getComponent<Rendering>()->getScaledSize();
                sf::RectangleShape rectangle(sf::Vector2f(size.first, size.second));
                rectangle.setPosition(static_cast<float>(trans->getX()) - (size.first / 2.0f), static_cast<float>(trans->getY()) - (size.second / 2.0f));
                rectangle.setFillColor(sf::Color(255, 255, 255, 64));
                texture.draw(rectangle);
            }
        }
    }

    if (render_hitboxes_) {
        for (auto it : type_color_map) {
            renderHitboxes(texture, it.first, it.second);
        }
    } else if (collideable_render_) {
        renderSingleHitbox(texture, collideable_render_,
                type_color_map.at(static_cast<int>(collideable_render_->getType())).second);
        collideable_render_.reset();
    }

    if (render_entrances_) {
        renderEntrances(texture, System::IWorldRead::getEntrances());
    }

    texture.display();
    window.draw(sf::Sprite(texture.getTexture()));

    // TODO Single texture glitches out, why?
    sf::RenderTexture texture_2;
    texture_2.create(window_size.first, window_size.second);
    texture_2.clear(sf::Color(0, 0, 0, 0));
    texture_2.setView(view);
    drawCameraBoundaries(texture_2);

    texture_2.display();
    window.draw(sf::Sprite(texture_2.getTexture()));
}

void EditorRender::drawCameraBoundaries(sf::RenderTarget& target) {
    auto viewport = System::getCamera()->getView();
    sf::View view = {{viewport.x_pos, viewport.y_pos}, {viewport.width, viewport.height}};

    auto view_center = view.getCenter();
    auto view_size = view.getSize();

    auto view_left = view_center.x - (view_size.x / 2.0f);
    auto view_right = view_center.x + (view_size.x / 2.0f);
    auto view_top = view_center.y - (view_size.y / 2.0f);
    auto view_bottom = view_center.y + (view_size.y / 2.0f);

    auto camera = System::getCamera()->getCameraBox();

    target.setView(view);

    drawLine(camera.left_margin, view_top, camera.left_margin, view_bottom, target);
    drawLine(camera.right_margin, view_top, camera.right_margin, view_bottom, target);
    drawLine(view_left, camera.top_margin, view_right, camera.top_margin, target);
    drawLine(view_left, camera.bottom_margin, view_right, camera.bottom_margin, target);
}

void EditorRender::setEditorEnvironment(std::weak_ptr<EditorEnvironment> editor_env) {
    editor_env_ = editor_env;
}

void EditorRender::addEntity(std::weak_ptr<Rendering> entity) {
    render_.addEntity(entity);
}

void EditorRender::setBackground(std::string background) {
    render_.setBackground(background);
}

void EditorRender::setPlayer(std::weak_ptr<Rendering> entity) {
    render_.setPlayer(entity);
}

void EditorRender::renderLayer(sf::RenderTarget& target, int layer) {
    render_.renderLayer(target, 0.0f, layer);
}

void EditorRender::setParallaxEnabled(bool enable) {
    render_.parallax_enabled_ = enable;
}

bool EditorRender::getParallaxEnabled() {
    return render_.parallax_enabled_;
}

void EditorRender::setWindowSize(sf::RenderWindow& window, int width, int height) {
    render_.setWindowSize(window, width, height);
}

void EditorRender::setCameraBox(Camera::CameraBoundingBox camera_box) {
    if (camera_box.left_margin > camera_box.right_margin ||
        camera_box.top_margin > camera_box.bottom_margin) {
        LOGW("Invalid camera margins");
        return;
    }

    System::IWorldModify::loadCameraData(camera_box);
    System::getCamera()->setCameraBox(camera_box);
}

void EditorRender::clearShaders() {
    render_.clearShaders();
}

void EditorRender::addShader(std::shared_ptr<ShaderHandle> shader, int layer) {
    render_.addShader(shader, layer);
}

void EditorRender::addGlobalShader(std::shared_ptr<ShaderHandle> shader) {
    render_.addGlobalShader(shader);
}

void EditorRender::renderCollideable(std::shared_ptr<Collideable> coll) {
    collideable_render_ = coll;
}
