#pragma once

#include <memory>

#include "render.h"
#include "camera.h"
#include "cutscene_handler.h"
#include "world.h"
#include "input_event.h"
#include "environment.h"

class System {
  public:
    static void setRender(std::shared_ptr<IRender> render);
    static std::shared_ptr<IRender> getRender();

    static void setCamera(std::shared_ptr<Camera> camera);
    static std::shared_ptr<Camera> getCamera();

    static std::shared_ptr<CutsceneHandler> getCutscene();

    static std::shared_ptr<Input> getInput();

    static void setEnvironment(std::shared_ptr<Environment> environment);
    static std::shared_ptr<Environment> getEnvironment();

    // World Interfaces
    class IWorldRead {
      public:
        static const std::vector<std::weak_ptr<const Collideable>>& getCollideables(Collideable::CollisionType coll_type);

        static util::Point getPlayerPosition();

        static std::weak_ptr<const Player> getPlayer();

        static std::string getCurrentRoomName();

        static std::vector<util::Point> getEntrances();
    };

    class IWorldModify {
      public:
        static void addEntity(std::shared_ptr<BaseEntity> entity, std::optional<std::string> condition = std::nullopt);
        static void removeEntity(std::shared_ptr<BaseEntity> entity);

        static void triggerConditionalEvents(std::string condition);

        static void addCollideable(std::shared_ptr<Collideable> collideable);

        static void loadWorldFromJson(nlohmann::json j);
        static void loadWorldFromFile(std::string file);

        static nlohmann::json saveWorldToJson();
        static void saveWorldToFile(std::string file);

        static void update();

        static void clearWorld();

        static std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects();
        static std::weak_ptr<Player> getPlayer();

        static void loadRoom(std::string room_name, int entrance_id);

        static void setEntrance(int entrance_id);

        static void loadCameraData(Camera::CameraBoundingBox camera_box);
    };

  private:
    static System& getInstance();

    std::shared_ptr<IRender> render_instance_ = std::make_shared<Render>();
    std::shared_ptr<Camera> camera_instance_ = std::make_shared<Camera>();
    std::shared_ptr<CutsceneHandler> cutscene_handler_ = std::make_shared<CutsceneHandler>();
    std::shared_ptr<World> world_ = std::make_shared<World>();
    std::shared_ptr<Input> input_ = std::make_shared<Input>();
    std::shared_ptr<Environment> environment_ = std::make_shared<Environment>();
};
