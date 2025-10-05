#include <SFML/Graphics.hpp>

/* Game class for 
    Window creation
    Creation of graphic display
    Handle user inputs
    Deal with the user inputs
    Display game objects on the screen
*/
class Game { 
    public:
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;
        Game();
        void run();
    
    private:
        void processEvents();
        void update();
        void render();

        sf::RenderWindow _window;
        sf::RectangleShape _background[60][32];
        sf::RectangleShape _sprites;
        // sf::RenderWindow _sprites;
        // sf::RenderWindow _menu;
};

// Create the window and player
Game::Game() : _window(sf::VideoMode({1920u, 1080u}), "CMake SFML Project") {
    for (int i = 0; i < 60;i++) {
        for (int j = 0; j < 32; j++) {
            _background[i][j].setSize(sf::Vector2f(32,32));
            if ((i+j)%2)
                _background[i][j].setFillColor(sf::Color(40,40,40));
            else _background[i][j].setFillColor(sf::Color(20,20,20));
            _background[i][j].setPosition({static_cast<float>(i)*32, static_cast<float>(j)*32});
        }
    }
    _sprites.setSize(sf::Vector2f(24,24));
    _sprites.setFillColor(sf::Color(223, 71, 127));
    _sprites.setPosition({1920/2-28, 1024/2-28});
}

// Main game loop
void Game::run() {
    while (_window.isOpen()) {
        processEvents();
        update();
        render(); 
    }
}

// Handle user inputs
void Game::processEvents() {
    while (std::optional event = _window.pollEvent()) {
        if ((event->is<sf::Event::Closed>()) or ((event->getIf<sf::Event::KeyPressed>()) and (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))) {
            _window.close();
            break;
        } else if ((event->getIf<sf::Event::KeyPressed>())) {
            switch (event->getIf<sf::Event::KeyPressed>()->code) {
                case sf::Keyboard::Key::W:
                    _sprites.move(sf::Vector2f(0,-32));
                    break;
                case sf::Keyboard::Key::A:
                    _sprites.move(sf::Vector2f(-32,0));
                    break;
                case sf::Keyboard::Key::S:
                    _sprites.move(sf::Vector2f(0,32));
                    break;
                case sf::Keyboard::Key::D:
                    _sprites.move(sf::Vector2f(32,0));
                    break;
                default:
                    break;
            }
        }
    }
}

// actual game
void Game::update() {}

// Render game to screen
void Game::render() {
    _window.clear();
    for (int i = 0; i < 60; i++) {
        for (int j = 0; j < 32; j++) {
            _window.draw(_background[i][j]);
        }
    }
    _window.draw(_sprites);
    _window.display();
}

int main()
{
    Game game;
    game.run();

    return 0;
}
