#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <time.h>
#include "Collision.h"
//#include <SFML/System.hpp>

class Food {
    public:
        static sf::RenderWindow* window;
        sf::CircleShape food;
        int lifeTime = 1000;
        bool alive = true;

        Food() : food(10) {
            food.setFillColor(sf::Color(214,6,78,255));
            food.setPosition(rand() % (window->getSize().x - 10), rand() % (window->getSize().y - 10));
        }
        void draw (){
            if(lifeTime <= 0){
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
        std::vector<sf::Vector2f> directions;
        std::vector<int> indexes;
        int currentIndex = 0;
        int changeDirectionTime = 100;
        int changeDirectionTimer = changeDirectionTime;
        sf::CircleShape bubble;
        int foodEaten = 0;
        int foodCost = 2;
        bool alive = true;
        int lifeTime = 1000;

        Cell() : bubble(10), directions(1, sf::Vector2f(0.1, 0)), indexes(1, 0) {
            bubble.setFillColor(sf::Color::Green);
            bubble.setPosition(rand() % window->getSize().x, rand() % window->getSize().y);
            bubble.setOutlineColor(sf::Color::White);
        }
        
        Cell(sf::Vector2f position) : bubble(10) {
            bubble.setFillColor(sf::Color::Green);
            bubble.setPosition(position);
            bubble.setOutlineColor(sf::Color::White);
        }

        void update() {
            if(directions.size() == 0) {
                return;
            }
            if(currentIndex >= indexes.size()){
                currentIndex = 0;
            }
            sf::Vector2f direction = directions[indexes[currentIndex]];
            bubble.move(direction.x, direction.y);
            changeDirectionTimer--;
            if(changeDirectionTimer <= 0){
                changeDirectionTimer = changeDirectionTime;
                currentIndex++;
            }

            if (bubble.getPosition().x < 0 || bubble.getPosition().x > window->getSize().x || bubble.getPosition().y < 0 || bubble.getPosition().y > window->getSize().y) {
                lifeTime = 0;
                //bubble.setPosition(window->getSize().x/2, window->getSize().y/2);
            }
        }
        
        void MutateDC() {
            float x = (rand() % 1001) / 1000.0 * (rand() % 2 ? -1 : 1);
            float y = (rand() % 1001) / 1000.0 * (rand() % 2 ? -1 : 1);
            directions.push_back(sf::Vector2f(x, y));
        }
        
        void MutateDM() {
            if(directions.size() == 0) {
                return;
            }
            float x = (rand() % 1001) / 1000.0 * (rand() % 2 ? -1 : 1);
            float y = (rand() % 1001) / 1000.0 * (rand() % 2 ? -1 : 1);
            directions[rand() % directions.size()] = sf::Vector2f(x, y);
        }

        void MutateRD() {
            if(directions.size() == 0) {
                return;
            }
            directions.erase(directions.begin() + rand() % directions.size());
        }
        
        void MutateIC() {
            if(directions.size() == 0) {
                return;
            }
            indexes.push_back(rand() % directions.size());
        }
        
        void MutateIM() {
            if(directions.size() == 0) {
                return;
            }
            indexes[rand() % indexes.size()] = rand() % directions.size();
        }
        
        void MutateRI() {
            if(indexes.size() == 0) {
                return;
            }
            indexes.erase(indexes.begin() + rand() % indexes.size());
        }

        void MutateCM() {
            changeDirectionTime = rand() % 1001;
        }
        
        void duplicate() {
            if(!(cells->size() < 100)){
                std::cout << "Max cells reached" << std::endl;
                return;
            }
            Cell newCell = Cell(bubble.getPosition());
            newCell.directions = directions;
            newCell.indexes = indexes;
            int mutationRate = 10;
            if(rand() % 101 < mutationRate){
                newCell.MutateDM();
                std::cout << "DM" << std::endl;
            }
            if(rand() % 101 < mutationRate){
                newCell.MutateDC();
                std::cout << "DM" << std::endl;
            }
            if(rand() % 101 < mutationRate){
                newCell.MutateIM();
                std::cout << "DM" << std::endl;
            }
            if(rand() % 101 < mutationRate){
                newCell.MutateIC();
                std::cout << "DM" << std::endl;
            }
            if(rand() % 101 < mutationRate){
                newCell.MutateCM();
                std::cout << "DM" << std::endl;
            }
            cells->push_back(newCell);
            std::cout << "Duplicated" << std::endl;
        }
        
        void draw () {
            if(lifeTime <= 0){
                alive = false;
                return;
            }
            update();
            window->draw(this->bubble);
            for(int i = 0; i < food->size(); i++){
                if(Collision::CircleTestShape(this->bubble, (*food)[i].food)){
                    (*food).erase((*food).begin() + i);
                    foodEaten++;
                    if(foodEaten >= foodCost){
                        duplicate();
                        foodEaten -= foodCost;
                    }
                }
            }
            lifeTime--;
        }
        ~Cell(){
        }
};

sf::RenderWindow* Cell::window = nullptr;
std::vector<Food>* Cell::food = nullptr;
std::vector<Cell>* Cell::cells = nullptr;

int main(){
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    window.setFramerateLimit(50);

    Food::window = &window;
    std::vector<Food> food;
    int foodLimit = 1000;
    int foodTimerValue = 1;
    int foodTimer = foodTimerValue;
    
    Cell::window = &window;
    Cell::food = &food;
    std::vector<Cell> cells;
    Cell::cells = &cells;

    for(int i = 0; i < 10; i++){
        cells.push_back(*(new Cell()));
    }

    Cell* selecteCell = nullptr;

    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed){
                bool found = false;
                for(auto& x : cells){
                    if(x.bubble.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)){
                        std::cout << "in\n";
                        found = true;
                        if(selecteCell != nullptr){
                            selecteCell->bubble.setOutlineThickness(0);
                            // delete selecteCell;
                        }
                        
                        selecteCell = &x;
                        x.bubble.setOutlineThickness(1);
                        break;
                    }
                }
                if(!found){
                    if(selecteCell != nullptr){
                        selecteCell->bubble.setOutlineThickness(0);
                        // delete selecteCell;
                    }
                    
                    selecteCell = nullptr;
                }
            }
        }

        if(foodTimer == 0){
            foodTimer = foodTimerValue;
            if(food.size() < foodLimit){
                food.push_back(*(new Food()));
            }
        }
        foodTimer--;

        window.clear();
        //window.draw(b);
        for(int i = 0; i < food.size(); i++){
            if(!food[i].alive){
                food.erase(food.begin() + i);
                i--;
                continue;
            }
            food[i].draw();
        }
        for(int i = 0; i < cells.size(); i++){
            if(!cells[i].alive){
                if(selecteCell == &cells[i]){
                    selecteCell->bubble.setOutlineThickness(0);
                    // delete selecteCell;
                    selecteCell = nullptr;
                }
                cells.erase(cells.begin() + i);
                i--;
                continue;
            }
            cells[i].draw();
        }
        window.display();
    }
}