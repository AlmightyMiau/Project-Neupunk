#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib> // rand
#include <cstring> // memcpy in class Action
#include <cmath>   // sin and cos
#include <iostream> // debugging

struct Consts {
    static const int windowX = 60;
    static const int windowY = 32;
    static const int gridX = 54;
    static const int gridY = 28;
    static const int gridOffset = 1;
    static const int itemSize = 24;
};

enum GAMESTATES {
    MAINMENU,
    PLAYING,
    GAMEOVER,
    EXIT
};

class GridItem {
    public:
        static const int gridSize = 32; // size of each square on the grid

        GridItem(int x = 0, int y = 0, int size = Consts::itemSize, sf::Color color = sf::Color::Magenta);

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

GridItem::GridItem(int x, int y, int size, sf::Color color) : item(sf::Vector2f(Consts::itemSize,Consts::itemSize)) {
    this->x=x;
    this->y=y;
    this->size=size;

    item.setSize(sf::Vector2f(size,size));
    item.setFillColor(color);
    item.setOrigin({(this->gridSize-this->size*1.f)/2,(this->gridSize-this->size*1.f)/2});
}

void GridItem::draw(sf::RenderWindow& window) {
    // gridSize (where it go) + (gridSize - size) (the grid offset) 
    float scale = gridSize * 1.f;
    item.setPosition(sf::Vector2f((x + Consts::gridOffset) * scale + (gridSize - size), (y + Consts::gridOffset) * scale + (gridSize - size)));
    window.draw(item);
}

// Player :3
class Player {
    public:
        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;
        Player();

        int getLength() {return snake.size();}

        void moveSnake();
        bool checkHitTail();
        bool checkHitWall();
        void sizeUp();
        bool checkApple(sf::Vector2f applePos);
        void reset();

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
    snake.push_back(GridItem(Consts::gridX/2, Consts::gridY/2, Consts::itemSize, sf::Color(71, 223, 127)));
}

void Player::processEvents(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::Up:
            if (direction != 3)
                direction = 1;
            break;
        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::Left:
            if (direction != 4)
            direction = 2;
            break;
        case sf::Keyboard::Key::S:
        case sf::Keyboard::Key::Down:
            if (direction != 1)
                direction = 3;
            break;
        case sf::Keyboard::Key::D:
        case sf::Keyboard::Key::Right:
            if (direction != 2)
                direction = 4;
            break;
        case sf::Keyboard::Key::C: // CHEAT (add snake length)
            sizeUp();
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

void Player::sizeUp() {
    snake.push_back(GridItem(tail.x, tail.y, Consts::itemSize, sf::Color(71, 223, 127)));
}

// Check if collided with apple, and add new segment if it did
bool Player::checkApple(sf::Vector2f applePos) {
    if (snake[0].getPosition() == applePos) {
        sizeUp();
        return true;
    }
    return false;
}

// Get rid of the snake
void Player::reset() {
    while (snake.size() > 0) {
        snake.pop_back();
    }
    snake.push_back(GridItem(Consts::gridX/2, Consts::gridY/2, Consts::itemSize, sf::Color(71, 223, 127)));
    direction = 0;
}

// move the snake
// w:1, a:2, s:3, d:4
// arrows also
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
        Apple(int x = 0, int y = 0, int size = Consts::itemSize, sf::Color color = sf::Color::Magenta);
        void newApple();
};
Apple::Apple(int x, int y, int size, sf::Color color) {
    newApple();
}
void Apple::newApple() {
    setX(rand()%Consts::gridX);
    setY(rand()%Consts::gridY);
}

class MainMenu {
    public:
        MainMenu(const MainMenu&) = delete;
        MainMenu& operator=(const MainMenu&) = delete;
        MainMenu(sf::Font& font);

        void setGameState (GAMESTATES g) {gameState = g;};
        void processEvents(sf::Keyboard::Key key);
        GAMESTATES update() {return gameState;}
        void draw(sf::RenderWindow& window);

    private:
        sf::Text title;
        sf::Text playButton;
        sf::Text exitButton;
        GAMESTATES gameState;
};

MainMenu::MainMenu(sf::Font& font) : title(font, "Title", 30), playButton(font, "Play Game", Consts::itemSize), exitButton(font, "Exit", Consts::itemSize) {
    title.setPosition({(Consts::gridX+Consts::gridOffset)*GridItem::gridSize+8, (2+Consts::gridOffset)*GridItem::gridSize});
    title.setFillColor(sf::Color::Red);

    playButton.setPosition({(Consts::gridX+Consts::gridOffset)*GridItem::gridSize+8, (2+Consts::gridOffset)*GridItem::gridSize});
    playButton.setFillColor(sf::Color::Magenta);

    exitButton.setPosition({(Consts::gridX+Consts::gridOffset)*GridItem::gridSize+8, (2+Consts::gridOffset)*GridItem::gridSize});
    exitButton.setFillColor(sf::Color::Magenta);

    gameState = MAINMENU;
}

void MainMenu::processEvents(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::Space:
            gameState = PLAYING;
            break;
        case sf::Keyboard::Key::Escape:
            gameState = EXIT;
            break;
        default:
            break;
    }
}
void MainMenu::draw(sf::RenderWindow& window) {
    window.draw(title);
    window.draw(playButton);
    window.draw(exitButton);
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

        void Exit() {_window.close();}
        void GameOver() {
            gameState = MAINMENU; 
            mainMenu.setGameState(MAINMENU);
            _player.reset();
            scoreDisplay.setString("Length : " + std::to_string(_player.getLength()));
        }

        void render();

        sf::RenderWindow _window;

        sf::RectangleShape backgroundSprite; 
        sf::Texture backgroundTexture;

        Player _player;
        Apple _apple;

        GAMESTATES gameState;

        // hud/ui
        sf::Font font;
        sf::Text scoreDisplay;
        MainMenu mainMenu;
};

bool Player::checkHitWall() {
    sf::Vector2f head = snake[0].getPosition();
    if (head.x < 0 || head.x > Consts::gridX*32 || head.y < 0 || head.y > Consts::gridY*32)
        return true;
    return false;
}


// Create the window and player
Game::Game() : _window(sf::VideoMode({1920u, 1080u}), "Kept you waiting huh?"), font("FreePixel.ttf"), scoreDisplay(font, "Length: 1", Consts::itemSize), mainMenu(font) {
    gameState = MAINMENU;
    // create 2x2 checker image (light/dark)
    sf::Image img({2,2}, sf::Color(20,20,20));
    img.setPixel({1, 0}, sf::Color(40,40,40)); // light
    img.setPixel({0, 1}, sf::Color(40,40,40)); // light

    if (!backgroundTexture.loadFromImage(img)) _window.close();
    backgroundTexture.setRepeated(true);

    sf::Vector2u win = _window.getSize();
    backgroundSprite.setSize({(Consts::gridX)*GridItem::gridSize, (Consts::gridY)*GridItem::gridSize});
    backgroundSprite.setPosition({Consts::gridOffset*GridItem::gridSize, Consts::gridOffset*GridItem::gridSize}); // move the game 1 square away from the top left

    // set texture rect so the tiny texture repeats to fill the whole window
    backgroundSprite.setTexture(&backgroundTexture);
    backgroundSprite.setTextureRect(sf::IntRect({0,0},{Consts::gridX, Consts::gridY}));

    scoreDisplay.setPosition({(Consts::gridX+Consts::gridOffset)*GridItem::gridSize+8, (2+Consts::gridOffset)*GridItem::gridSize});
    scoreDisplay.setFillColor(sf::Color::Magenta);
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

/*
    HOW TO GET MOUSE INPUT FOR CLICKING BUTTONS ?????????
*/
// Handle user inputs
void Game::processEvents() {
    while (const std::optional<sf::Event> event = _window.pollEvent()) {
        if ((event->is<sf::Event::Closed>()) or ((event->getIf<sf::Event::KeyPressed>()) and (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))) {
            _window.close();
            break;
        } else if (event->getIf<sf::Event::KeyPressed>()) {
            switch (gameState) {
                case MAINMENU:
                    mainMenu.processEvents(event->getIf<sf::Event::KeyPressed>()->code);
                    break;
                case PLAYING:
                    _player.processEvents(event->getIf<sf::Event::KeyPressed>()->code);
                    break;
            }
        }
    }
}

// actual game
void Game::update(sf::Time deltaTime) {
    switch (gameState) {
        case MAINMENU:
            gameState = mainMenu.update(); // i dont think this does anything
            break;
        case PLAYING:
            if (_player.update(deltaTime)) GameOver();
            if (_player.checkApple(_apple.getPosition())) {
                _apple.newApple();
                scoreDisplay.setString("Length : " + std::to_string(_player.getLength()));
            }
            break;
        default:
            std::cout << "fuck";
            break;
    }
}

// Render game to screen
void Game::render() {
    _window.clear();

    switch (gameState) {
        case MAINMENU:
            mainMenu.draw(_window);
            break;
        case PLAYING:
            _window.draw(backgroundSprite);
            _window.draw(scoreDisplay);
            _apple.draw(_window);
            _player.draw(_window);
            break;
    }
    
    _window.display();
}

int main()
{
    srand(0);
    Game game;
    game.run(20);

    return 0;
}
