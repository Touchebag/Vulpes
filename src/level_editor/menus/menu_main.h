#include <imgui.h>
#include <imgui-SFML.h>

#include "editor_loop/editor_environment.h"

#include "top_menu.h"

namespace menu {

void renderMenus(sf::RenderWindow& window, std::shared_ptr<EditorEnvironment> editor_env);

} // menu
