#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <time.h>
#include <algorithm>
#include <math.h>
#include "Collision.h"
//#include <SFML/System.hpp>

class Food {
public:
    static sf::RenderWindow* window;
    sf::CircleShape food;
    int lifeTime = 1000;
    bool alive = true;

    Food() : food(10) {
        food.setFillColor(sf::Color(214, 6, 78, 255));
        food.setPosition(rand() % (window->getSize().x - 10), rand() % (window->getSize().y - 10));
    }
    void draw() {
        if (lifeTime <= 0) {
            alive = false;
            return;
        }
        window->draw(food);
        lifeTime--;
    }
};

sf::RenderWindow* Food::window = nullptr;

class Cell {
public:
    static sf::RenderWindow* window;
    static std::vector<Food>* food;
    static std::vector<Cell>* cells;
    std::string id;
    std::vector<sf::Vector2f> directions;
    std::vector<int> indexes;
    int speed = 5;
    int currentIndex = 0;
    int changeDirectionTime = 100;
    int changeDirectionTimer = changeDirectionTime;
    int duplicateTime = 50;
    int duplicateTimer = duplicateTime;
    int consumeFoodTime = 500;
    int consumeFoodTimer = consumeFoodTime;
    sf::CircleShape bubble;
    int foodEaten = 2;
    int foodCost = 2;
    bool alive = true;
    int lifeTime = 10000;
    int size = 10;

    Cell() : bubble(10), directions(1, sf::Vector2f(0.1, 0)), indexes(1, 0) {
        bubble.setFillColor(sf::Color::Green);
        bubble.setPosition(rand() % window->getSize().x, rand() % window->getSize().y);
        bubble.setOutlineColor(sf::Color::White);
        setId();
    }

    Cell(sf::Vector2f position) : bubble(size) {
        bubble.setFillColor(sf::Color::Green);
        bubble.setPosition(position);
        bubble.setOutlineColor(sf::Color::White);
        setId();
    }
    void setId() {
        id += (char)('A' + rand() % ('Z' - 'A'));
        id += (char)('A' + rand() % ('Z' - 'A'));
        id += (char)('A' + rand() % ('Z' - 'A'));
        id += (char)('A' + rand() % ('Z' - 'A'));
        id += (char)('A' + rand() % ('Z' - 'A'));
    }

    void update() {
        if (directions.empty()) {
            return;
        }
        if (currentIndex >= indexes.size()) {
            currentIndex = 0;
        }
        sf::Vector2f direction = directions[indexes[currentIndex]];
        bubble.move(direction.x * speed, direction.y * speed);
        changeDirectionTimer--;
        if (changeDirectionTimer <= 0) {
            changeDirectionTimer = changeDirectionTime;
            currentIndex++;
        }

        if (bubble.getPosition().x < 0 || bubble.getPosition().x > window->getSize().x || bubble.getPosition().y < 0 || bubble.getPosition().y > window->getSize().y) {
            lifeTime = 0;
        }
        duplicateTimer--;
        consumeFoodTimer--;
        consumeFood();
        catchFood();
        if (foodEaten == 0) {
            lifeTime -= 5;
        }
        if (foodEaten >= foodCost && duplicateTimer <= 0) {
            duplicate();
            foodEaten -= foodCost;
            duplicateTimer = duplicateTime;
        }
        if (duplicateTimer < 0) {
            duplicateTimer = 0;
        }
        lifeTime--;
        if (lifeTime < 0) {
            lifeTime = 0;
        }
    }

    void SetSelected() {
        bubble.setOutlineThickness(3);
    }
    void SetUnselected() {

        bubble.setOutlineThickness(0);
    }

    void MutateDC() {
        float x = (rand() % 1001) / 1000.0 * (rand() % 2 ? -1 : 1);
        float y = (rand() % 1001) / 1000.0 * (rand() % 2 ? -1 : 1);
        directions.push_back(sf::Vector2f(x, y));
    }

    void MutateDM() {
        if (directions.empty()) {
            return;
        }
        float x = (rand() % 1001) / 1000.0 * (rand() % 2 ? -1 : 1);
        float y = (rand() % 1001) / 1000.0 * (rand() % 2 ? -1 : 1);
        directions[rand() % directions.size()] = sf::Vector2f(x, y);
    }

    void MutateRD() {
        if (directions.empty()) {
            return;
        }
        directions.erase(directions.begin() + rand() % directions.size());
    }

    void MutateIC() {
        if (directions.empty()) {
            return;
        }
        indexes.push_back(rand() % directions.size());
    }

    void MutateIM() {
        if (directions.empty()) {
            return;
        }
        indexes[rand() % indexes.size()] = rand() % directions.size();
    }

    void MutateRI() {
        if (indexes.empty()) {
            return;
        }
        indexes.erase(indexes.begin() + rand() % indexes.size());
    }

    void MutateCM() {
        changeDirectionTime = rand() % 1001;
    }

    void MutateSM() {
        size = 10 + ((rand() % 10));
        foodCost = 2 * (size * size) / 100;
        bubble.setRadius(size);
    }

    void duplicate() {
        if (cells->size() > 100) {
            return;
        }
        Cell newCell = Cell(bubble.getPosition());
        newCell.directions = directions;
        newCell.indexes = indexes;
        int mutationRate = 10;
        if (rand() % 101 < mutationRate) {
            newCell.MutateDM();
        }
        if (rand() % 101 < mutationRate) {
            newCell.MutateDC();
        }
        if (rand() % 101 < mutationRate) {
            newCell.MutateIM();
        }
        if (rand() % 101 < mutationRate) {
            newCell.MutateIC();
        }
        if (rand() % 101 < mutationRate) {
            newCell.MutateCM();
        }
        if (rand() % 101 < 1) {
            newCell.MutateSM();
        }
        cells->push_back(newCell);
    }

    void catchFood() {
        for (int i = 0; i < food->size(); i++) {
            if (Collision::CircleTestShape(this->bubble, (*food)[i].food)) {
                food->erase(food->begin() + i);
                foodEaten += 2;
            }
        }
    }

    void consumeFood() {
        if (consumeFoodTimer <= 0) {
            foodEaten -= ceil(0.2 * foodCost);
            if (foodEaten < 0) {
                foodEaten = 0;
            }
            consumeFoodTimer = consumeFoodTime;
        }
    }
    void draw() {
        if (lifeTime <= 0) {
            alive = false;
            return;
        }
        update();
        window->draw(this->bubble);
    }
};

sf::RenderWindow* Cell::window = nullptr;
std::vector<Food>* Cell::food = nullptr;
std::vector<Cell>* Cell::cells = nullptr;
std::vector<Cell>::iterator* selected = nullptr;

int main() {
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    window.setFramerateLimit(50);

    sf::Font font;
    font.loadFromFile("Fifteen-Regular.ttf");

    sf::Text text("", font);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(3);

    Food::window = &window;
    std::vector<Food> food;
    int foodLimit = 1000;
    int foodTimerValue = 1;
    int foodTimer = foodTimerValue;

    std::vector<Cell> cells;

    Cell::window = &window;
    Cell::food = &food;
    Cell::cells = &cells;

    for (int i = 0; i < 10; i++) {
        cells.push_back(*(new Cell()));
    }

    for (int i = 0; i < foodLimit; i++) {
        food.push_back(*(new Food()));
    }
    Cell defaultSelectedCell = Cell();
    std::string selecteCell = "";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                bool found = false;
                for (int i = 0; i < cells.size(); i++) {
                    if (cells[i].bubble.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        std::cout << "in\n";
                        found = true;
                        auto it = std::find_if(cells.begin(), cells.end(), [selecteCell](const Cell& x) {
                            return (x.id == selecteCell);
                            });
                        if (it != cells.end()) {
                            it->SetUnselected();
                        }
                        //x.SetSelected();
                        selecteCell = cells[i].id;
                        cells[i].SetSelected();
                        break;
                    }
                }
                if (!found) {
                    std::cout << selecteCell << "out\n";
                    auto it = std::find_if(cells.begin(), cells.end(), [selecteCell](const Cell& x) {
                        return (x.id == selecteCell);
                        });
                    if (it != cells.end()) {
                        it->SetUnselected();
                        // delete selecteCell;
                    }

                    selecteCell = "";
                    std::cout << "out\n";
                }
            }
        }

        if (foodTimer == 0) {
            foodTimer = foodTimerValue;
            if (food.size() < foodLimit) {
                food.push_back(*(new Food()));
            }
        }
        foodTimer--;

        window.clear();
        for (int i = 0; i < food.size(); i++) {
            if (!food[i].alive) {
                food.erase(food.begin() + i);
                i--;
                continue;
            }
            food[i].draw();
        }
        auto it = std::find_if(cells.begin(), cells.end(), [selecteCell](const Cell& x) {
            return (x.id == selecteCell);
            });
        if (it != cells.end() && !(it->alive)) {
            std::cout << "Unselected\n";
            it->SetUnselected();
            selecteCell = "";
        }
        for (int i = 0; i < cells.size(); i++) {
            if (!cells[i].alive) {
                cells.erase(cells.begin() + i);
                i--;
                continue;
            }
            cells[i].draw();
        }
        it = std::find_if(cells.begin(), cells.end(), [selecteCell](const Cell& x) {
            return (x.id == selecteCell);
            });
        if (it != cells.end()) {
            it->draw();
            text.setPosition(0, 0);
            text.setString("Id: " + it->id);
            window.draw(text);

            float height = text.getGlobalBounds().height;
            float space = 5;
            text.move(0, height + space);
            text.setString("Time to consume Food: " + std::to_string(it->size));
            window.draw(text);

            text.move(0, height + space);
            text.setString("Remaining lifetime: " + std::to_string(it->lifeTime));
            window.draw(text);

            text.setString("Food: " + std::to_string(it->foodEaten));
            text.move(0, height + space);
            window.draw(text);

            text.move(0, height + space);
            text.setString("Food cost: " + std::to_string(it->foodCost));
            window.draw(text);

            text.move(0, height + space);
            text.setString("Time to consume Food: " + std::to_string(it->consumeFoodTimer));
            window.draw(text);

            text.move(0, height + space);
            text.setString("Time to duplicate: " + std::to_string(it->duplicateTimer));
            window.draw(text);
        }

        window.display();
    }
}