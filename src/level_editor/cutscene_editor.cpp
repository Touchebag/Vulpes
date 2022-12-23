#include "cutscene_editor.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include "system/system.h"
#include "base_entity.h"
#include "editor_render.h"

#include "utils/log.h"

#define PHYSICS_FRAME_RATE 60
#define MS_PER_FRAME 1000 / PHYSICS_FRAME_RATE

void renderSeekBar(const std::string& cutscene_name, int& frame_count) {
    auto cutscene = System::getCutscene();

    ImGui::SetNextWindowPos({0, 970});

    ImGui::Begin("Seek Bar", nullptr, 0
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoBackground
            );

    ImGui::SetNextItemWidth(950);
    ImGui::SliderInt("", &frame_count, 0, static_cast<int>(cutscene->getLength()));

    ImGui::End();

    int i = static_cast<int>(cutscene->getCurrentFrame());
    if (i < frame_count) {
        cutscene->fastForward(static_cast<unsigned int>(frame_count - i));
    } else if (i > frame_count) {
        cutscene = Cutscene::loadCutscene(cutscene_name);
        System::setCutscene(cutscene);
        cutscene->fastForward(static_cast<unsigned int>(frame_count));
    }
}

int cutsceneEditorMain(sf::RenderWindow& window) {
    std::string cutscene_name = "boxes";

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

    System::setCutscene(Cutscene::loadCutscene(cutscene_name));

    int frame_count = 0;

    sf::Time time_rendered;
    sf::Clock render_clock;
    render_clock.restart();

    bool auto_play = false;

    while (window.isOpen()) {
        time_rendered += render_clock.getElapsedTime();
        render_clock.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && window.hasFocus()){
            window.close();
        }

        auto cutscene = System::getCutscene();

        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            switch (event.type) {
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Key::Right) {
                        auto_play = false;
                        cutscene->fastForward(1);
                        frame_count = static_cast<int>(cutscene->getCurrentFrame());
                    } else if (event.key.code == sf::Keyboard::Key::Left) {
                        auto_play = false;
                        // Don't go further back than 0
                        if (auto i = cutscene->getCurrentFrame(); i != 0) {
                            cutscene = Cutscene::loadCutscene(cutscene_name);
                            System::setCutscene(cutscene);
                            cutscene->fastForward(i - 1);
                            frame_count = static_cast<int>(cutscene->getCurrentFrame());
                        }
                    } else if (event.key.code == sf::Keyboard::Key::Space) {
                        auto_play = !auto_play;
                    }
                    break;
                default:
                    break;
            }
        }

        while (time_rendered.asMilliseconds() >= (MS_PER_FRAME)) {
            if (auto_play) {
                cutscene->update();
                frame_count = static_cast<int>(cutscene->getCurrentFrame());
            }

            time_rendered -= sf::milliseconds(MS_PER_FRAME);
        }

        ImGui::SFML::Update(window, delta_clock.restart());

        window.clear();
        System::getRender()->render(window, 0.0);

        renderSeekBar(cutscene_name, frame_count);
        ImGui::SFML::Render(window);

        window.display();
    }

    // Remove cutscene to avoid it being drawn over main editor
    System::clearCutscene();

    System::setRender(old_render);
    System::setWorld(old_world);

    return 0;
}
