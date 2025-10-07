#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> // rand
#include <cstring> // memcpy in class Action
#include <cmath>   // sin and cos


// Player :3
class Player : public sf::Drawable {
    public:
        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;
        Player();

        // to handle both overloads of sf::Transformable::setPosition(), which can use two floats, or a Vector2f
        // we need to template for multiple args, and just pass all of them directly to the _shape.setPosition(), 
        // regardless of what is given.  
        template<typename ... Args>
        void setPosition(Args&& ... args) {
            _shape.setPosition(std::forward<Args>(args)...);
        }

        void update(sf::Time deltaTime);
        void processEvents();
    
    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        sf::RectangleShape _shape;
        bool isMoving;
        int rotation;
        sf::Vector2f _velocity;
};

Player::Player() : _shape(sf::Vector2f(24,24)) {
    _shape.setFillColor(sf::Color(71, 223, 127));
    _shape.setOrigin({4,4});
}

void Player::update(sf::Time deltaTime) {
    float seconds = deltaTime.asSeconds();
    if (rotation != 0) {
        sf::Angle angle = sf::degrees(rotation*180*seconds);
        _shape.rotate(angle);
    }
    if (isMoving) {
        sf::Angle angle = _shape.getRotation();
        _velocity += sf::Vector2f(std::cos(angle.asRadians()), std::sin(angle.asRadians())) * 60.f * seconds;
    }
    _shape.move(seconds * _velocity);
}

void Player::processEvents() {
    isMoving = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    rotation = 0;
    rotation -= sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    rotation += sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_shape, states);
}

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
        void run(int fps);
        static const int gridX = 60;
        static const int gridY = 32;
    
    private:
        void processEvents();
        void update(sf::Time deltaTime);
        void moveSnake(int direction);
        void checkHitTail();
        void checkHitWall();
        void checkApple(sf::Vector2f lastPos);
        void newApple();
        void render();

        sf::RenderWindow _window;
        sf::RectangleShape _background[gridX][gridY];
        std::vector<sf::RectangleShape> _sprites;
        sf::Vector2f _LastTailPos;
        sf::RectangleShape _apple;
        int appleNum = 0;
        int _direction = 0; 

        Player _player;
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
    sf::RectangleShape newPlayer;
    _sprites.push_back(newPlayer);
    _sprites[0].setSize(sf::Vector2f(24,24));
    _sprites[0].setFillColor(sf::Color(71, 223, 127));
    _sprites[0].setPosition({1920/2-28, 1024/2-28});

    _apple.setSize(sf::Vector2f(24,24));
    _apple.setFillColor(sf::Color(223, 71, 127));
    _apple.setPosition({1920/4-28, 1024/4-28});
}

// Main game loop
void Game::run(int fps) {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time TimePerFrame = sf::seconds(1.f/fps);

    while (_window.isOpen()) {
        processEvents();
        bool repaint = false;

        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            repaint = true;
            update(TimePerFrame);
        }
        if(repaint)
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
                    _direction = 1;
                    break;
                case sf::Keyboard::Key::A:
                    _direction = 2;
                    break;
                case sf::Keyboard::Key::S:
                    _direction = 3;
                    break;
                case sf::Keyboard::Key::D:
                    _direction = 4;
                    break;
                default:
                    break;
            }
        }
    }
    _player.processEvents();
}

// actual game
void Game::update(sf::Time deltaTime) {
    // Kill snake if hit tail
    moveSnake(_direction);
    checkHitTail();
    checkHitWall();
    checkApple(_LastTailPos);
    _player.update(deltaTime);
}

void Game::checkHitTail() {
    for (int i = 1; i < _sprites.size(); i++) {
        if (_sprites[0].getPosition() == _sprites[i].getPosition())
            _window.close();
    }
}

void Game::checkHitWall() {
    sf::Vector2f head = _sprites[0].getPosition();
    if (head.x < 0 || head.x > gridX*32 || head.y < 0 || head.y > gridY*32)
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
    _apple.setPosition({static_cast<float>((rand()%gridX)*32+4), static_cast<float>((rand()%gridY)*32+4)});
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
    _window.draw(_player);
    _window.display();
}

int main()
{
    srand(0);
    Game game;
    game.run(10);

    return 0;
}
