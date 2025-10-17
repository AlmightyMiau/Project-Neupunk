#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> // rand
#include <cstring> // memcpy in class Action
#include <cmath>   // sin and cos
#include <iostream> // debugging

class GridItem {
    public:
        static const int gridX = 60;
        static const int gridY = 32;
        static const int gridSize = 32; // size of each square on the grid

        GridItem(int x = 0, int y = 0, int size = 24, sf::Color color = sf::Color::Magenta);

        int getX() {return x;}
        int getY() {return y;}
        int getSize() {return size;}
        sf::Vector2f getPosition() {return {x*gridSize*1.f+gridSize-size, y*gridSize*1.f+gridSize-size};}

        void setX(int val) {x=val;}
        void setY(int val) {y=val;}
        void setPos(int newx, int newy) {x=newx; y=newy;}
        void setSize(int val) {size=val;}
        void move(int dx, int dy) {x+=dx; y+=dy;}

        void draw(sf::RenderWindow& window);

    private:
        sf::RectangleShape item;
        int x;    // x location on the grid
        int y;    // y location on the grid
        int size; // size of item on the grid, 24
};

GridItem::GridItem(int x, int y, int size, sf::Color color) : item(sf::Vector2f(24,24)) {
    this->x=x;
    this->y=y;
    this->size=size;

    item.setSize(sf::Vector2f(size,size));
    item.setFillColor(color);
    item.setOrigin({(this->gridSize-this->size*1.f)/2,(this->gridSize-this->size*1.f)/2});
}

void GridItem::draw(sf::RenderWindow& window) {
    item.setPosition(sf::Vector2f(x*gridSize*1.f+gridSize-size, y*gridSize*1.f+gridSize-size));
    window.draw(item);
}

// Player :3
class Player {
    public:
        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;
        Player();

        void moveSnake();
        bool checkHitTail();
        bool checkHitWall();
        bool checkApple(sf::Vector2f applePos);

        bool update(sf::Time deltaTime);
        void processEvents(sf::Keyboard::Key key);
        void draw(sf::RenderWindow& window) {
            for (int i = 0; i < snake.size(); i++) {
                snake[i].draw(window);
            }
        }
    
    private:
        std::vector<GridItem> snake;
        int direction;
        sf::Vector2f tail; // location of last position
};

Player::Player() {
    snake.push_back(GridItem(GridItem::gridX/2, GridItem::gridY/2, 24, sf::Color(71, 223, 127)));
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
        snake.push_back(GridItem(tail.x, tail.y, 24, sf::Color(71, 223, 127)));
        return true;
    }
    return false;
}

// move the snake
// w:1, a:2, s:3, d:4
void Player::moveSnake() {
    tail = snake[snake.size()-1].getPosition();
    for (int i = snake.size()-1; i >= 1; i--) {
        snake[i].setPos(snake[i-1].getX(), snake[i-1].getY());
    }
    switch (direction) {
        case 1: // up
            snake[0].move(0,-1);
            break;
        case 2: // left
            snake[0].move(-1,0);
            break;
        case 3: // down
            snake[0].move(0,1);
            break;
        case 4: // right
            snake[0].move(1,0);
            break;
        default:
            break;
    }
}

class Apple : public GridItem {
    public:
        Apple(int x = 0, int y = 0, int size = 24, sf::Color color = sf::Color::Magenta);
        void newApple();
};
Apple::Apple(int x, int y, int size, sf::Color color) {
    newApple();
}
void Apple::newApple() {
    setX(rand()%gridX);
    setY(rand()%gridY);
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
    
    private:
        void processEvents();
        void update(sf::Time deltaTime);

        void GameOver() {_window.close();};

        void render();

        sf::RenderWindow _window;
        // sf::RectangleShape _background[gridX][gridY]; // do this next
        sf::RectangleShape _checkerSprite; // rectangle sized to window, with repeated texture
        sf::Texture _checkerTexture;

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
    if (head.x < 0 || head.x > GridItem::gridX*32 || head.y < 0 || head.y > GridItem::gridY*32)
        return true;
    return false;
}


// Create the window and player
Game::Game() : _window(sf::VideoMode({1920u, 1080u}), "Kept you waiting huh?") {
    // create 2x2 checker image (light/dark)
    sf::Image img({2,2}, sf::Color(20,20,20));
    img.setPixel({1, 0}, sf::Color(40,40,40)); // light
    img.setPixel({0, 1}, sf::Color(40,40,40)); // light

    if (!_checkerTexture.loadFromImage(img)) _window.close();
    _checkerTexture.setRepeated(true);

    sf::Vector2u win = _window.getSize();
    _checkerSprite.setSize({(GridItem::gridX)*GridItem::gridSize, (GridItem::gridY)*GridItem::gridSize});

    // set texture rect so the tiny texture repeats to fill the whole window
    _checkerSprite.setTexture(&_checkerTexture);
    _checkerSprite.setTextureRect(sf::IntRect({0,0},{GridItem::gridX, GridItem::gridY}));
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
    // for (int i = 0; i < 60; i++) {
    //     for (int j = 0; j < 32; j++) {
    //         _window.draw(_background[i][j]);
    //     }
    // }
    _window.draw(_checkerSprite);
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
