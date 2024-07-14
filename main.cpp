#include "util/things.hpp"




int main() {


    sf::RenderWindow window(sf::VideoMode(800, 700), "Mines");
    sf::Font font;
    font.loadFromFile("../assets/Hack-Bold.ttf");

    th::game_state state = th::game_state::in_game;
    th::DrawableItemsManager manager(window, state, font);

    th::MineFiled mine_field(window, state, font);

    while (window.isOpen()) {


        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            manager.distibute_event(event);

        }

        window.clear();
        //

        manager.draw_items();

        //
        window.display();

    }

    return 0;
}