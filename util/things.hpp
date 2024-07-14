#include <SFML/Graphics.hpp>
// #include <SFML/Audio.hpp>
#include <vector>
#include <list>
#include <random>
#include <iostream>
#include <unordered_map>

namespace th {

    int tile_size = 38;
    int field_start = 20;
    int field_x = 16;
    int field_y = 16;
    int mines_count = 40;

    bool is_near(int n, int center) {

        bool res = false;
        if (n == center) return true;
        if (center % 16 != 15) if (n == center + 1) res = true;
        if (center % 16 != 0) if (n == center - 1) res = true;
        if (center / 16 != 0) if (n == center - 16) res = true;
        if (center / 16 != 15) if (n == center + 16) res = true;
        if (center % 16 != 15 && center / 16 != 0) if (n == center - 15) res = true;
        if (center % 16 != 0 && center / 16 != 0) if (n == center - 17) res = true;
        if (center % 16 != 15 && center / 16 != 15) if (n == center + 17) res = true;
        if (center % 16 != 0 && center / 16 != 15) if (n == center + 15) res = true;
        return res;
    }

    enum class game_state {
        loose,
        win,
        in_game
    };

    class Drawable {

    public:

        virtual void draw() = 0;

        virtual game_state process_event(const sf::Event& event) = 0;
    };

    struct Tile {
        int number;
        bool is_mine;
        int mines_around = 0;
        bool is_discovered = false;
        bool flag = false;


        void draw(sf::RenderWindow& window, std::unordered_map<std::string, sf::Texture>& textures) {
            if (is_discovered) {
                sf::RectangleShape tile({tile_size, tile_size});
                if (is_mine) {
                    tile.setTexture(&textures["mine.png"]);
                } else if (mines_around) {
                    tile.setTexture(&textures[char(mines_around + 48) + std::string(".png")]);
                    
                } else {
                    tile.setTexture(&textures["blank.png"]);
                }
                
                tile.setPosition({field_start + (number % field_x) * tile_size, field_start + (number / field_x) * tile_size});
                window.draw(tile);
            } else {
                sf::RectangleShape tile({tile_size, tile_size});

                if (flag) {
                    tile.setTexture(&textures["flag.png"]);
                } else {
                    tile.setTexture(&textures["undiscovered.png"]);
                }
                tile.setPosition({field_start + (number % field_x) * tile_size, field_start + (number / field_x) * tile_size});
                window.draw(tile);
            }
        }
    };




    class MineFiled : public Drawable {

        sf::RenderWindow& window;
        bool mines_generated = false;
        std::vector<std::vector<Tile>> field;
        std::unordered_map<std::string, sf::Texture> textures;
        int flags_placed = 0;
        sf::Font& font;
        game_state& state;
        // sf::SoundBuffer sound_buff;

        void discover(int n) {
            if (!get_tile(n).is_discovered ) {
                if (get_tile(n).flag) {
                    get_tile(n).flag = false;
                    --flags_placed;
                }
                get_tile(n).is_discovered = true;

                bool win_move = true;
                for (auto& i : field) {
                    for (auto& j : i) {
                        if ((!j.is_mine) && (!j.is_discovered)) {
                            win_move = false;
                            break;
                        }
                    }
                    if (!win_move) break;
                }
                if (win_move) win();

                if (get_tile(n).mines_around == 0) {
                    if (n % 16 != 15) discover(n + 1);
                    if (n % 16 != 0) discover(n - 1);
                    if (n / 16 != 0) discover(n - 16);
                    if (n / 16 != 15) discover(n + 16);
                    if (n % 16 != 15 && n / 16 != 0) discover(n - 15);
                    if (n % 16 != 0 && n / 16 != 0) discover(n - 17);
                    if (n % 16 != 15 && n / 16 != 15) discover(n + 17);
                    if (n % 16 != 0 && n / 16 != 15) discover(n + 15);
                }
            }
        }

        void on_left_click(int x, int y) {
            int number = (y - field_start) / tile_size * field_x + (x - field_start) / tile_size;
            if (!mines_generated) {
                generate_mines(number);
                mines_generated = true;
                // debug();
            }
            discover(number);
            Tile& tile = get_tile(number);
            if (tile.is_mine) {
                loose();
            }

        }

        void on_right_click(int x, int y) {
            
            int number = (y - field_start) / tile_size * field_x + (x - field_start) / tile_size;
            if (!get_tile(number).is_discovered) {
                if (get_tile(number).flag) {
                    --flags_placed;
                    get_tile(number).flag = false;
                } else {
                    if (flags_placed < mines_count) {
                        ++flags_placed;
                        get_tile(number).flag = true;
                    }
                }
            }
        }

        void debug() {
            for (auto& i: field) {
                for (auto j : i) {
                    std::cout << (j.is_mine ? 'x' : char(j.mines_around + 48)) << ' ';
                }
                std::cout << '\n';
            }
        }

                Tile& get_tile(int num) {return field[num / field_x][num % field_x];}

        void generate_mines(int first_clck_num) {
            srand(time(0));
            for (int i = 0; i < mines_count; ++i) {
                bool mine_placed;
                do {
                    int tile_n = rand() % (field_x * field_y);
                    if (get_tile(tile_n).is_mine || is_near(tile_n, first_clck_num)) {
                        mine_placed = false;                

                    } else {
                        get_tile(tile_n).is_mine = true;
                        mine_placed = true;
                        if (tile_n % 16 != 15) ++get_tile(tile_n + 1).mines_around;
                        if (tile_n % 16 != 0) ++get_tile(tile_n - 1).mines_around;
                        if (tile_n / 16 != 0) ++get_tile(tile_n - 16).mines_around;
                        if (tile_n / 16 != 15) ++get_tile(tile_n + 16).mines_around;
                        if (tile_n % 16 != 15 && tile_n / 16 != 0) ++get_tile(tile_n - 15).mines_around;
                        if (tile_n % 16 != 0 && tile_n / 16 != 0) ++get_tile(tile_n - 17).mines_around;
                        if (tile_n % 16 != 15 && tile_n / 16 != 15) ++get_tile(tile_n + 17).mines_around;
                        if (tile_n % 16 != 0 && tile_n / 16 != 15) ++get_tile(tile_n + 15).mines_around;
                    }
                } while (!mine_placed);
            }
        }

        void loose() {
            state = game_state::loose;
            for (auto& i : field) {
                for (auto& j : i) {
                    if (j.is_mine) {
                        j.is_discovered = true;
                    }
                }
            } 
            std::cout << "Ochen bad\n";
        }

        void win() {
            state = game_state::win;
            for (auto& i : field) {
                for (auto& j : i) {
                    if (j.is_mine) {
                        if (!j.flag) {
                            j.flag = true;
                            ++flags_placed;
                        }
                    }
                }
            } 
            std::cout << "Congratulations\n";            
        }


    public:

        MineFiled(sf::RenderWindow& window, game_state& state, sf::Font& font): window(window), state(state), font(font) {
            field.resize(field_y);
            for (auto& i : field) {
                i.resize(field_x);
            }

            int curr = 0;
            for (auto& i: field) {
                for (auto& j : i) {
                    j.number = curr++;

                }
            }

            std::vector<std::string> texture_names({"1.png", "2.png", "3.png", "4.png", "5.png", "6.png", "7.png", "8.png", "blank.png", "flag.png", "undiscovered.png", "mine.png"});
            std::string assets_folder = "../assets/";


            for (auto name : texture_names) {
                sf::Texture texture;
                if (!texture.loadFromFile(assets_folder + name)) {
                    std::cout << "Texture bad loading\n";
                }
                textures[name] = texture;
            }

            font.loadFromFile(assets_folder + std::string("Hack-Bold.ttf"));
            // sound_buff.loadFromFile(assets_folder + std::string("boom_sound.wav"));
        }


        void draw() override{
            for (auto& i: field) {
                for (auto& j : i) {
                    j.draw(window, textures);
                }
            }
            
            sf::RectangleShape hor_line({field_x * tile_size, 1});
            hor_line.setFillColor(sf::Color::Black);
            for (int hor = 0; hor <= field_x; ++hor) {
                hor_line.setPosition({field_start, field_start + hor * tile_size});
                window.draw(hor_line);

            }

            sf::RectangleShape ver_line({1, field_y * tile_size});
            ver_line.setFillColor(sf::Color::Black);
            for (int ver = 0; ver <= field_y; ++ver) {
                ver_line.setPosition({field_start + ver * tile_size, field_start});
                window.draw(ver_line);                   
            }

            sf::RectangleShape flags_placed_field({100, 36});
            flags_placed_field.setPosition({field_start + field_x*tile_size + tile_size, field_start});
            flags_placed_field.setFillColor(sf::Color(130,130,130));
            sf::Text text;
            text.setString(std::to_string(flags_placed) + std::string("/" + std::to_string(mines_count)));
            text.setPosition(flags_placed_field.getPosition());
            text.setFont(font);
            window.draw(flags_placed_field);
            window.draw(text);
        }

        game_state process_event(const sf::Event& event) override {
            switch (event.type){
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        on_left_click(event.mouseButton.x, event.mouseButton.y);
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        on_right_click(event.mouseButton.x, event.mouseButton.y);
                    }
                    break;
                case sf::Event::KeyPressed:
                    // std::cout << event.key.code << '\n';
                    switch (event.key.code) {
                        case 17:
                            refresh_field();
                            break;
                        case 2:
                            debug();
                            break;
                    }
            }
            return state;
        }

        void refresh_field() {
            mines_generated = false;
            flags_placed = 0;
            state = game_state::in_game;

            for (auto& i: field) {
                for (auto& j : i) {
                    j.flag = false;
                    j.is_mine = false;
                    j.mines_around = 0;
                    j.is_discovered = false;
                }
            }
        }
    };

    class Button : public Drawable {

        game_state& state;
        sf::RenderWindow& window;
        sf::RectangleShape outer;
        sf::Text text;
        sf::Vector2f position = {300, 300};
        sf::Vector2f scale = {90, 40};
    public:

        Button(sf::RenderWindow& window, sf::Font& font, game_state& state): window(window), state(state) {
            text.setString("Retry");
            text.setFont(font);
            text.setPosition(position);
            outer.setPosition(position);
            outer.setSize(scale);
            outer.setFillColor(sf::Color(200, 200, 200));
        }

        void draw() override {
            window.draw(outer);
            window.draw(text);
        }

        game_state process_event(const sf::Event& event) override {
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (event.mouseButton.x >= position.x && event.mouseButton.x <= (position.x + scale.x) && event.mouseButton.y >= position.y && event.mouseButton.y <= (position.y + scale.y)) {
                        return game_state::in_game;
                    }
                }
            }
            return state;
        }
    };

    class Menu : public Drawable {
        sf::RenderWindow& window;
        sf::RectangleShape outer;
        sf::Text text;
        sf::Vector2f position = {170, 200};
        sf::Vector2f scale = {350, 200};
        Button retry_button;
        game_state& state;
    
    public:
        Menu(sf::RenderWindow& window, game_state& state, sf::Font& font): window(window), retry_button(window, font, state), state(state) {
            text.setFont(font);
            text.setPosition({position.x + 30, position.y + 30});
            outer.setPosition(position);
            outer.setSize(scale);

        }

        void draw() override {
            if (state == game_state::loose) {
                text.setString("Try again :(");
                outer.setFillColor(sf::Color(200, 150, 150));
            } else {
                text.setString("Congartulations!!");
                outer.setFillColor(sf::Color(150, 200, 150));    
            }
            window.draw(outer);
            window.draw(text);
            retry_button.draw();
        }

        game_state process_event(const sf::Event& event) override {
            return retry_button.process_event(event);
        }

    };



    class DrawableItemsManager {
        
        MineFiled mine_field;
        Menu menu;
        game_state& state;

    public:

        DrawableItemsManager(sf::RenderWindow& window, game_state& state, sf::Font& font): mine_field(window, state, font), menu(window, state, font), state(state) {}

        void distibute_event(sf::Event& event) {
            if (state == game_state::in_game) {
                state = mine_field.process_event(event);
            } else {
                state = menu.process_event(event);
                if (state == game_state::in_game) {
                    mine_field.refresh_field();
                }
            }
        }

        void draw_items() {
            mine_field.draw();
            if (state != game_state::in_game) {
                menu.draw();
            }
        }

    };

}