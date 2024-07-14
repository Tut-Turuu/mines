#include "util/things.hpp"




int main() {


    sf::RenderWindow window(sf::VideoMode(800, 700), "Mines");

    
    th::DrawableItemsManager manager;

    th::MineFiled mine_field(window);

    manager.push_front(&mine_field);
    
    
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

        mine_field.draw();

        //
        window.display();

    }

    return 0;
}