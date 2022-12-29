#include <imgui.h>
#include <imgui-SFML.h>

#include <functional>

class TopMenu {
  public:
    void draw(sf::RenderWindow& window);

    void addMenu(const std::string& name, std::function<void(void)> entry);
    void clearMenus();

  private:
    std::vector<std::pair<std::string, std::function<void(void)>>> menus_;
};
