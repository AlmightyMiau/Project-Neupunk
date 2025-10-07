#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> // rand

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
        void moveSnake(int direction);
        void checkHitTail();
        void checkHitWall();
        void checkApple(sf::Vector2f lastPos);
        void newApple();
        void render();

        sf::RenderWindow _window;
        static const int _gridX = 60;
        static const int _gridY = 32;
        sf::RectangleShape _background[_gridX][_gridY];
        std::vector<sf::RectangleShape> _sprites;
        sf::Vector2f _LastTailPos;
        sf::RectangleShape _apple;
        int appleNum = 0;
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
    sf::RectangleShape player;
    _sprites.push_back(player);
    _sprites[0].setSize(sf::Vector2f(24,24));
    _sprites[0].setFillColor(sf::Color(71, 223, 127));
    _sprites[0].setPosition({1920/2-28, 1024/2-28});

    _apple.setSize(sf::Vector2f(24,24));
    _apple.setFillColor(sf::Color(223, 71, 127));
    _apple.setPosition({1920/4-28, 1024/4-28});
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
                    moveSnake(1);
                    break;
                case sf::Keyboard::Key::A:
                    moveSnake(2);
                    break;
                case sf::Keyboard::Key::S:
                    moveSnake(3);
                    break;
                case sf::Keyboard::Key::D:
                    moveSnake(4);
                    break;
                default:
                    break;
            }
        }
    }
}

// actual game
void Game::update() {
    // Kill snake if hit tail
    checkHitTail();
    checkHitWall();
    checkApple(_LastTailPos);
}

void Game::checkHitTail() {
    for (int i = 1; i < _sprites.size(); i++) {
        if (_sprites[0].getPosition() == _sprites[i].getPosition())
            _window.close();
    }
}

void Game::checkHitWall() {
    sf::Vector2f head = _sprites[0].getPosition();
    if (head.x < 0 || head.x > _gridX*32 || head.y < 0 || head.y > _gridY*32)
        _window.close();
}

// move the snake
// w:1, a:2, s:3, d:4
void Game::moveSnake(int direction) {
    _LastTailPos = _sprites[_sprites.size() - 1].getPosition();
    for (int i = _sprites.size()-1; i >= 1; i--) {
        _sprites[i].setPosition(_sprites[i-1].getPosition());
    }
    switch (direction) {
        case 1:
            _sprites[0].move(sf::Vector2f(0,-32));
            break;
        case 2:
            _sprites[0].move(sf::Vector2f(-32,0));
            break;
        case 3:
            _sprites[0].move(sf::Vector2f(0,32));
            break;
        case 4:
            _sprites[0].move(sf::Vector2f(32,0));
            break;
        default:
            break;
    }
}

// Check if collided with apple, and add new segment if it did
void Game::checkApple(sf::Vector2f lastPos) {
    if (_sprites[0].getPosition() == _apple.getPosition()) {
        sf::RectangleShape newTail;
        _sprites.push_back(newTail);
        _sprites[_sprites.size()-1].setSize(sf::Vector2f(24,24));
        _sprites[_sprites.size()-1].setFillColor(sf::Color(71, 223, 127));
        _sprites[_sprites.size()-1].setPosition(lastPos);
        newApple();
    }
}

// new apple
void Game::newApple() {
    _apple.setPosition({static_cast<float>((rand()%_gridX)*32+4), static_cast<float>((rand()%_gridY)*32+4)});
}

// Render game to screen
void Game::render() {
    _window.clear();
    for (int i = 0; i < 60; i++) {
        for (int j = 0; j < 32; j++) {
            _window.draw(_background[i][j]);
        }
    }
    _window.draw(_apple);
    for (int i = 0; i < _sprites.size(); i++)
        _window.draw(_sprites[i]);
    _window.display();
}

int main()
{
    srand(0);
    Game game;
    game.run();

    return 0;
}
