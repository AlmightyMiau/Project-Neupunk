#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> // rand
#include <cstring> // memcpy in class Action
#include <cmath>   // sin and cos


// Player :3
class Player {
    public:
        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;
        Player();

        // to handle both overloads of sf::Transformable::setPosition(), which can use two floats, or a Vector2f
        // we need to template for multiple args, and just pass all of them directly to the _shape.setPosition(), 
        // regardless of what is given.  
        template<typename ... Args>
        void setPosition(Args&& ... args) {
            snake[0].setPosition(std::forward<Args>(args)...);
        }

        void moveSnake();
        bool checkHitTail();
        bool checkHitWall();
        bool checkApple(sf::Vector2f applePos);

        bool update(sf::Time deltaTime);
        void processEvents(sf::Keyboard::Key key);
        void draw(sf::RenderWindow& window) {for (sf::RectangleShape segment : snake) window.draw(segment);}
    
    private:
        std::vector<sf::RectangleShape> snake;
        int direction;
        sf::Vector2f tail; // location of last position
};

Player::Player() {
    snake.push_back(sf::RectangleShape(sf::Vector2f(24,24)));
    snake[0].setFillColor(sf::Color(71, 223, 127));
    snake[0].setOrigin({4,4});
    snake[0].setPosition({1920/2-24, 1024/2-24});
}

void Player::processEvents(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::W:
            direction = 1;
            break;
        case sf::Keyboard::Key::A:
            direction = 2;
            break;
        case sf::Keyboard::Key::S:
            direction = 3;
            break;
        case sf::Keyboard::Key::D:
            direction = 4;
            break;
        default:
            break;
    }
}

bool Player::update(sf::Time deltaTime) {
    moveSnake();
    if (checkHitTail()) return true;
    if (checkHitWall()) return true;
    return false;
}

bool Player::checkHitTail() {
    for (int i = 1; i < snake.size(); i++) {
        if (snake[0].getPosition() == snake[i].getPosition())
            return true;
    }
    return false;
}

// Check if collided with apple, and add new segment if it did
bool Player::checkApple(sf::Vector2f applePos) {
    if (snake[0].getPosition() == applePos) {
        snake.push_back(sf::RectangleShape(sf::Vector2f(24,24)));
        snake[snake.size()-1].setFillColor(sf::Color(71, 223, 127));
        snake[snake.size()-1].setOrigin({4,4});
        snake[snake.size()-1].setPosition(tail);
        return true;
    }
    return false;
}

// move the snake
// w:1, a:2, s:3, d:4
void Player::moveSnake() {
    tail = snake[snake.size()-1].getPosition();
    for (int i = snake.size()-1; i >= 1; i--) {
        snake[i].setPosition(snake[i-1].getPosition());
    }
    switch (direction) {
        case 1:
            snake[0].move(sf::Vector2f(0,-32));
            break;
        case 2:
            snake[0].move(sf::Vector2f(-32,0));
            break;
        case 3:
            snake[0].move(sf::Vector2f(0,32));
            break;
        case 4:
            snake[0].move(sf::Vector2f(32,0));
            break;
        default:
            break;
    }
}

class Apple {
    public:
        Apple(const Apple&) = delete;
        Apple& operator=(const Apple&) = delete;
        Apple();

        sf::Vector2f getPosition() {return _apple.getPosition();}
        
        void newApple();
        void draw(sf::RenderWindow& window) {window.draw(_apple);}
    
    private:
        sf::RectangleShape _apple;
};
Apple::Apple() : _apple(sf::Vector2f(24,24)) {
    _apple.setFillColor(sf::Color(223, 71, 127));
    _apple.setOrigin({4,4});
    newApple();
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

        void GameOver() {_window.close();};

        void render();

        sf::RenderWindow _window;
        sf::RectangleShape _background[gridX][gridY]; // do this next

        std::vector<sf::RectangleShape> _sprites;
        sf::Vector2f _LastTailPos;
        int _direction = 0; 

        Player _player;
        Apple _apple;
        // sf::RenderWindow _sprites;
        // sf::RenderWindow _menu;
};

bool Player::checkHitWall() {
    sf::Vector2f head = snake[0].getPosition();
    if (head.x < 0 || head.x > Game::gridX*32 || head.y < 0 || head.y > Game::gridY*32)
        return true;
    return false;
}

// new apple
void Apple::newApple() {
    _apple.setPosition({((rand()%Game::gridX)*32.f+8), ((rand()%Game::gridY)*32.f+8)});
}

// Create the window and player
Game::Game() : _window(sf::VideoMode({1920u, 1080u}), "Kept you waiting huh?") {
    for (int i = 0; i < 60;i++) {
        for (int j = 0; j < 32; j++) {
            _background[i][j].setSize(sf::Vector2f(32,32));
            if ((i+j)%2)
                _background[i][j].setFillColor(sf::Color(40,40,40));
            else _background[i][j].setFillColor(sf::Color(20,20,20));
            _background[i][j].setPosition({static_cast<float>(i)*32, static_cast<float>(j)*32});
        }
    }
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
    while (const std::optional<sf::Event> event = _window.pollEvent()) {
        if ((event->is<sf::Event::Closed>()) or ((event->getIf<sf::Event::KeyPressed>()) and (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))) {
            _window.close();
            break;
        } else if (event->getIf<sf::Event::KeyPressed>()) {
            _player.processEvents(event->getIf<sf::Event::KeyPressed>()->code);
        }
    }
}

// actual game
void Game::update(sf::Time deltaTime) {
    if (_player.update(deltaTime)) GameOver();
    if (_player.checkApple(_apple.getPosition()))
        _apple.newApple();
}

// Render game to screen
void Game::render() {
    _window.clear();
    for (int i = 0; i < 60; i++) {
        for (int j = 0; j < 32; j++) {
            _window.draw(_background[i][j]);
        }
    }
    _apple.draw(_window);
    for (int i = 0; i < _sprites.size(); i++)
        _window.draw(_sprites[i]);
    _player.draw(_window);
    _window.display();
}

int main()
{
    srand(0);
    Game game;
    game.run(20);

    return 0;
}
