// Paul Kull, 2022

// TODO: Change position calculation to integral at time t method, since method atm relies on random sample times
#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <vector>

int G = 1;
int FPS = 30;
int maxX = 1200;
int maxY = 900;

class Body {
public:
    float _position[3];
    float _velocity[3];
    float _acceleration[3];
    float _mass;

    Body(float pos[3], float vel[3], float acc[3], float mass = 10) {
        for (int i = 0; i < 3; i++) {
            _position[i] = pos[i];
            _velocity[i] = vel[i];
            _acceleration[i] = acc[i];
            _mass = mass;
        }
    }

    void setAcceleration(float acc[3]) {
        for (int i = 0; i < 3; i++) {
            _acceleration[i] = acc[i];
        }
    }

    void setMass(float mass) {
        _mass = mass;
    }

    void addForce(float acc[3]) {
        for (int i = 0; i < 3; i++) {
            _acceleration[i] += acc[i];
        }
    }

    void setPosition(float pos[3]) {
        for (int i = 0; i < 3; i++) {
            _position[i] = pos[i];
        }
    }

    float getPos(int i) {
        return _position[i];
    }

    float getMass() {
        return _mass;
    }

    void update() {
        for (int i = 0; i < 3; i++) {
            _position[i] += _velocity[i];
            _velocity[i] += _acceleration[i];
            _acceleration[i] = 0;
        }
    }
};

class Simulation {
    std::vector<Body> _bodys;
    int _numBodys;
    int _spaceScaler;

public:

    Simulation() {
        float body1_pos[3] = { 800, 450, 100 };
        float body2_pos[3] = { 400, 450, 100 };
        float z[3] = { 350, 400, 100 };
        float w[3] = { 400, 300, 100 };
        float nullvektor[3] = { 0, 0, 0 };
        float move_up[3] = { 0, 2, 0 };
        float move_down[3] = { 0, -2, 0 };
        _bodys.push_back(Body(body1_pos, move_up, nullvektor));
        _bodys.push_back(Body(body2_pos, move_down, nullvektor));
        // _bodys.push_back(Body(z, nullvektor, nullvektor));
        // _bodys.push_back(Body(w, nullvektor, nullvektor));
        _numBodys = 2;
        //_bodys[3].setMass(2);
        _spaceScaler = 1;
    }

    void printPositions() {
        for (int i = 0; i < _numBodys; i++) {
            std::cout << "Body" << i << ": " << _bodys[i].getPos(0) << " " << _bodys[i].getPos(1) << " " << _bodys[i].getPos(2) << "\t";
        }
        std::cout << "\n";
    }

    void calculateAccels() {
        float distance;
        float direction[3] = { 0.0, 0.0, 0.0 };
        float body1[3];
        float body2[3];
        float mass1;
        float mass2;
        for (int i = 0; i < _numBodys; i++) {
            for (int j = 0; j < 3; j++) {
                body1[j] = _bodys[i].getPos(j);
            }
            mass1 = _bodys[i].getMass();
            for (int ii = 0; ii < _numBodys; ii++) {
                if (i == ii) { continue; }
                for (int j = 0; j < 3; j++) {
                    body2[j] = _bodys[ii].getPos(j);
                }
                mass2 = _bodys[i].getMass();
                distance = sqrt(pow(body2[0] - body1[0], 2) + pow(body2[1] - body1[1], 2) + pow(body2[2] - body1[2], 2));
                for (int j = 0; j < 3; j++) {
                    if (distance > 0) {
                        direction[j] = (mass2 * G * (body2[j] - body1[j])) / (pow(std::max(distance, float(1)), 2 * _spaceScaler));
                    }
                    if (distance < 0) {
                        direction[j] = (mass2 * G * (body2[j] - body1[j])) / (pow(std::min(distance, float(-1)), 2 * _spaceScaler)); // mass1 cancels out
                    }
                    std::cout << "Acceleration of body" << i << ": " << direction[j] << "\t";
                }
            }
            std::cout << "\n";
            _bodys[i].addForce(direction);
            for (int j = 0; j < 3; j++) {
                direction[j] = 0;
            }
        }
    }

    int getNumBodys() {
        return _numBodys;
    }

    float getBodyMass(int i) {
        return _bodys[i].getMass();
    }

    void updatePositions() {
        for (int j = 0; j < _numBodys; j++) {
            _bodys[j].update();
        }
    }

    float getBodyCoordinate(int body, int coordinate) {
        return _bodys[body].getPos(coordinate);
    }

    bool addBody(float x, float y, float z) {
        if (x > maxX || x < 0 || y < 0 || y > maxY) {
            return 0;
        }
        float nullvektor[3] = { 0, 0, 0 };
        float position[3] = { x, y, z };
        _bodys.push_back(Body(position, nullvektor, nullvektor));
        _numBodys += 1;
        return 1;
    }
};

class Renderer {

    std::vector<sf::CircleShape> _shapes;
    int _numShapes;
    float _massSizeRatio = 60;
    sf::Color _ColorArray[6];
    int _numColors;
    sf::Sprite _background;
    sf::Texture _backgroundTexture;

public:
    Renderer(Simulation* sim) {
        _ColorArray[0] = sf::Color::Green;
        _ColorArray[1] = sf::Color::Red;
        _ColorArray[2] = sf::Color::Yellow;
        _ColorArray[3] = sf::Color::Blue;
        _ColorArray[4] = sf::Color::Cyan;
        _ColorArray[5] = sf::Color::Magenta;
        _numColors = 6;

        if (!_backgroundTexture.loadFromFile("C:/Users/paulk/source/repos/ThreeBodySim/ThreeBodySim/Resources/SpaceBackground.png"))
        {
            std::cout << "\nERROR LOADING BACKGROUND FILE\n";
        }
        _background = sf::Sprite(_backgroundTexture);
        _background.setPosition(sf::Vector2f(0, 0));
        sf::IntRect rect;
        rect.height = maxY;
        rect.width = maxX;
        rect.top = 0;
        rect.left = 0;
        _background.setTextureRect(rect);

        for (int i = 0; i < sim->getNumBodys(); i++) {
            _shapes.push_back(sf::CircleShape(sim->getBodyMass(i) * _massSizeRatio));
            _shapes[i].setFillColor(_ColorArray[i]);
        }
        _numShapes = sim->getNumBodys();
    }

    void updateGraphics(Simulation* sim) {
        // First Check whether or not we have the correct amount of circles
        if (_numShapes != sim->getNumBodys()) {
            if (_numShapes < sim->getNumBodys()) {
                // Not enough circles
                for (int i = 0; i < sim->getNumBodys() - _numShapes; i++) {
                    _shapes.push_back(sf::CircleShape(sim->getBodyMass(_numShapes) * _massSizeRatio));
                    _numShapes++;
                    _shapes[_numShapes - 1].setFillColor(_ColorArray[(_numShapes - 1) % _numColors]);
                }
            }
            else {
                // Too many circles
                for (int i = 0; i < _numShapes - sim->getNumBodys(); i++) {
                    _shapes.pop_back();
                    _numShapes--;
                }
            }
        }

        for (int i = 0; i < _numShapes; i++) {
            _shapes[i].setPosition(sf::Vector2f(sim->getBodyCoordinate(i, 0), sim->getBodyCoordinate(i, 1)));
            if (sim->getBodyCoordinate(i, 2) > 0) {
                _shapes[i].setRadius(sim->getBodyMass(i) * _massSizeRatio / (sim->getBodyCoordinate(i, 2) + 0.1));
            }
            else {
                _shapes[i].setRadius(0);
            }
        }
    }
    void draw(sf::RenderWindow* window) {
        // clear the window with black color
        window->clear(sf::Color::Black);
        window->draw(_background);
        for (int i = 0; i < _numShapes; i++) {
            window->draw(_shapes[i]);
        }
        // end the current frame
        window->display();

    }
    
};

class InputHandler {
    sf::RenderWindow* _window;
    Renderer* _ren;
    Simulation* _sim;
    bool _buttonIsPressed;
    bool _runSimulation;

public:
    InputHandler(sf::RenderWindow* window, Simulation* sim, Renderer* ren) {
        _window = window;
        _sim = sim;
        _ren = ren;
        _runSimulation = true;
        _buttonIsPressed = false;
    }

    bool handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            _window->close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (!_buttonIsPressed) {
                _runSimulation = !_runSimulation;
            }
            _buttonIsPressed = true;
            return _runSimulation;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (!_buttonIsPressed) {
                sf::Vector2i pos = sf::Mouse::getPosition(*_window);
                std::cout << "\n" << pos.x << " " << pos.y << "\n";
                bool worked = _sim->addBody(pos.x, pos.y, 100);
                if (!_runSimulation) {
                    _ren->updateGraphics(_sim);
                    _ren->draw(_window);
                }
            }
            _buttonIsPressed = true;
            return _runSimulation;
        }
        _buttonIsPressed = false;
        return _runSimulation;
    }

};


int main() {
    Simulation sim = Simulation();
    sf::VideoMode vm = sf::VideoMode();
    vm.width = maxX;
    vm.height = maxY;
    sf::RenderWindow window = sf::RenderWindow(vm, "Three Body Simulation");
    Renderer ren = Renderer(&sim);
    sf::Clock clock;
    InputHandler inputHandler = InputHandler(&window, &sim, &ren);
    bool runSimulation = true;
    // create the window
    // run the program as long as the window is open
    while (window.isOpen())
    {
        runSimulation = inputHandler.handleInput();

        if (clock.getElapsedTime().asMilliseconds() > 1000 / FPS && runSimulation) {
            clock.restart();
            sim.calculateAccels();
            sim.updatePositions();
            // sim.printPositions();

            // draw everything here...
            ren.updateGraphics(&sim);
            ren.draw(&window);
        }
    }
    return 0;
}