#include "cutscene_editor.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include "system/system.h"
#include "base_entity.h"
#include "editor_render.h"

#include "utils/log.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

void renderSeekBar(int& frame_count) {
    ImGui::SetNextWindowPos({0, 950});
    ImGui::SetNextWindowSize({1000, 500});

    ImGui::Begin("Seek Bar", nullptr, 0
            | ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoBackground
            );

    if (ImGui::BeginMenuBar()) {
        ImGui::SliderInt("", &frame_count, 0, 100);

        ImGui::EndMenuBar();
    }

    ImGui::End();

    auto cutscene = System::getCutscene();
    int i = static_cast<int>(cutscene->getCurrentFrame());
    if (i < frame_count) {
        cutscene->fastForward(static_cast<unsigned int>(frame_count - i));
    } else if (i > frame_count) {
        cutscene = Cutscene::loadCutscene("boxes");
        System::setCutscene(cutscene);
        cutscene->fastForward(static_cast<unsigned int>(frame_count));
    }
}

int cutsceneEditorMain(sf::RenderWindow& window) {
    ImGui::SFML::Init(window);
    sf::Clock delta_clock;

    auto old_render = System::getRender();
    auto old_world = System::getWorld();
    auto render = std::make_shared<Render>();
    auto world = std::make_shared<World>();

    System::setRender(render);
    System::setWorld(world);

    render->setWindowSize(window, 1000, 1000);

    window.setKeyRepeatEnabled(true);

    auto cutscene = Cutscene::loadCutscene("boxes");

    System::setCutscene(cutscene);

    int frame_count = 0;

    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && window.hasFocus()){
            break;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            switch (event.type) {
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Key::Right) {
                        cutscene->fastForward(1);
                        frame_count = static_cast<int>(cutscene->getCurrentFrame());
                    } else if (event.key.code == sf::Keyboard::Key::Left) {
                        auto i = cutscene->getCurrentFrame();
                        cutscene = Cutscene::loadCutscene("boxes");
                        System::setCutscene(cutscene);
                        cutscene->fastForward(i - 1);
                        frame_count = static_cast<int>(i - 1);
                    }
                    break;
                default:
                    break;
            }
        }

        ImGui::SFML::Update(window, delta_clock.restart());

        window.clear();
        System::getRender()->render(window, 0.0);

        renderSeekBar(frame_count);
        ImGui::SFML::Render(window);

        window.display();
    }

    // Remove cutscene to avoid it being drawn over main editor
    System::clearCutscene();

    System::setRender(old_render);
    System::setWorld(old_world);

    return 0;
}
